#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QToolButton>
#include <QSystemTrayIcon>
#include <QMenu>
#include <unordered_map>
#include <vector>
#include <QAbstractNativeEventFilter>
#include "utility.h"
#include "app_model.h"
#include "camera_opengl_item.h"

//#include <Eigen/Core>
//#include <pcl/point_types.h>
//#include <pcl/features/fpfh.h>
//#include <pcl/registration/ia_ransac.h>
//#include <pcl/features/normal_3d.h>
////#include <pcl/kdtree/kdtree_flann.h>
//#include <pcl/features/fpfh_omp.h> //包含fpfh加速计算的omp(多核并行计算)
//#include <pcl/registration/correspondence_estimation.h>
//#include <pcl/registration/correspondence_rejection_features.h> //特征的错误对应关系去除
//#include <pcl/registration/correspondence_rejection_sample_consensus.h> //随机采样一致性去除
//#include <pcl/filters/voxel_grid.h>
//#include <pcl/filters/approximate_voxel_grid.h>

#include <Eigen/Core>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/fpfh.h>
#include <pcl/registration/ia_ransac.h>

class QGraphicsScene;
class QGraphicsView;
class OGLGraphicsScene;
class OGLGraphicsView;
class QGraphicsItem;

typedef pcl::PointCloud<pcl::PointXYZ> pointcloud;
typedef pcl::PointCloud<pcl::Normal> pointnormal;
typedef pcl::PointCloud<pcl::FPFHSignature33> fpfhFeature;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent* event) override;
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void moveEvent(QMoveEvent* event);

private slots:
    void on_import1_btn_clicked();

    void on_import2_btn_clicked();

    void on_coarse_btn_clicked();

    void on_fine_btn_clicked();

    void on_fusion_btn_clicked();

    void on_global_btn_clicked();

    void on_checkBoxMesh_clicked(bool checked);

private:
    void load3DFile(CameraOpenGLItem::DisplayType e_type);
    fpfhFeature::Ptr compute_fpfh_feature(pointcloud::Ptr input_cloud, pcl::search::KdTree<pcl::PointXYZ>::Ptr tree);

    template<typename T>
    void displayTransform(const T& final_transformation);

    pcl::PointCloud<pcl::PointXYZ>::Ptr  mf_voxelDownSamplePointCloud(
        const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in, const float &voxelSideLength);

    pcl::PointCloud<pcl::PointXYZ>::Ptr mf_outlierRemovalPC(
        const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in, const float meanK = 8, const float std_dev = 2.5);

    pcl::PointCloud<pcl::Normal>::Ptr mf_computeNormals(const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in,
            const float &searchRadius);

    pcl::CorrespondencesPtr mf_estimateCorrespondences(const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud1,
            const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud2,
            const pcl::PointCloud<pcl::Normal>::Ptr &normals1,
            const pcl::PointCloud<pcl::Normal>::Ptr &normals2,
            const double &max_distance);

    pcl::PointCloud<pcl::PointXYZ>::Ptr mf_SVDInitialAlignment(const pcl::PointCloud<pcl::PointXYZ>::Ptr &source,
            const pcl::PointCloud<pcl::PointXYZ>::Ptr &target,
            pcl::CorrespondencesPtr correspondences,
            Eigen::Matrix4f &transformation_matrix);

    template <typename PointT>
    boost::shared_ptr<pcl::PointCloud<PointT>> mf_iterativeClosestPointFinalAlignment(
            const boost::shared_ptr<pcl::PointCloud<PointT>> &source,
            const boost::shared_ptr<pcl::PointCloud<PointT>> &target,
            const float &maxCorrespondenceDistance,
            Eigen::Matrix4f &icpTransformation);

    void mf_PoissonMeshes(
        pcl::PointCloud<pcl::PointXYZ>::Ptr mv_PointCloudInput, pcl::PolygonMesh::Ptr &mv_MeshesOutput);

    void mf_NormalEstimation(const pcl::PointCloud<pcl::PointXYZ>::Ptr &mv_PointCloudInput,
        pcl::PointCloud<pcl::PointNormal>::Ptr &mv_PointNormalOutput);
    
    int test();

private:
    Ui::MainWindow *ui;
    OGLGraphicsScene* m_scene;
    OGLGraphicsView* m_view;
    std::unordered_map<CameraOpenGLItem::DisplayType, QGraphicsItem*> m_Cam_items;

    int m_titlebar_height;

    std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> m_cp1;
    std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> m_cp2;

    pcl::PointCloud<pcl::PointXYZ>::Ptr m_cloud1;
    pcl::PointCloud<pcl::PointXYZ>::Ptr m_cloud2;

    pcl::PointCloud<pcl::PointXYZ>::Ptr m_alignedPCs;

    bool m_checked;
};

#endif // MAINWINDOW_H
