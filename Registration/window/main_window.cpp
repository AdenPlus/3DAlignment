#include "stdafx.h"
#include "main_window.h"
#include "ui_main_window.h"

#include <QString>
#include <QGraphicsView>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QToolButton>
#include <QFileDialog>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "camera_item.h"
#include "camera_opengl_item.h"
#include "align_text_item.h"
#include "ogl_graphicsscene.h"
#include "ogl_graphicsview.h"
//#include "camera_downward_item.h"

#include <pcl/common/common.h>
#include <pcl/io/io.h>
#include <pcl/io/obj_io.h>
#include <pcl/PolygonMesh.h>
#include <pcl/io/obj_io.h>
#include <pcl/point_cloud.h>
#include <pcl/registration/icp.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/registration/correspondence_estimation_backprojection.h>
#include <pcl/registration/correspondence_rejection_sample_consensus.h>
#include <pcl/surface/marching_cubes.h>
#include <pcl/surface/concave_hull.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/surface/poisson.h>
#include <pcl/filters/passthrough.h>
#include <pcl/surface/poisson.h>
#include <pcl/PCLPointCloud2.h>

static const QSize ITEM_SIZE = QSize(164, 90);

using namespace std;
using namespace pcl;
/**************************************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("3D Registration"));

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);

    m_scene = new OGLGraphicsScene(this);
    m_view = new OGLGraphicsView(m_scene,this);

    m_view->setScene(m_scene);

    ui->verticalLayout_GL->addWidget(m_view, 0);

	m_titlebar_height = 30;//style()->pixelMetric(QStyle::PM_TitleBarHeight)+3;

    m_cloud1 = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>());
    m_cloud2 = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>());

    m_alignedPCs = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>());

    connect(ui->checkBoxMesh, &QCheckBox::clicked, this, &MainWindow::on_checkBoxMesh_clicked);

    //connect(ui->fusion_btn, &QPushButton::clicked, this, &MainWindow::on_fusion_btn_clicked);
}

/**************************************************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/**************************************************************************************************/
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
}

/**************************************************************************************************/
void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
}

/**************************************************************************************************/
void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QMainWindow::mousePressEvent(event);
}

/**************************************************************************************************/
void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    QMainWindow::mouseMoveEvent(event);
}

/**************************************************************************************************/
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    QMainWindow::mouseReleaseEvent(event);
}

/**************************************************************************************************/
void MainWindow::moveEvent(QMoveEvent* event)
{
    QMainWindow::moveEvent(event);
}

/**************************************************************************************************/
void MainWindow::resizeEvent(QResizeEvent* event)
{
    if (m_scene)
    {
        m_view->setGeometry(ui->verticalLayout_GL->geometry());

        m_scene->setSceneRect(0, 0, ui->verticalLayout_GL->geometry().width(),
                              ui->verticalLayout_GL->geometry().height());

        //m_scene->setSceneRect(0, -this->height(), this->width(), this->height());
        //m_view->scale(1, -1);
    }

    QMainWindow::resizeEvent(event);
}

void MainWindow::on_import1_btn_clicked()
{
    load3DFile(CameraOpenGLItem::PairOne);
}

void MainWindow::on_import2_btn_clicked()
{
    load3DFile(CameraOpenGLItem::PairTwo);
}


void MainWindow::load3DFile(CameraOpenGLItem::DisplayType e_type)
{
    auto iter = m_Cam_items.find(e_type);
    if (iter != m_Cam_items.end())
    {
        std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> data;
        auto pCamItemGL = dynamic_cast<CameraOpenGLItem*>(iter->second);

        if( e_type == CameraOpenGLItem::Result)
            pCamItemGL->setAllPoints(data, data);
        else
            pCamItemGL->setPoints(data);
        
        pCamItemGL->update();
        m_Cam_items.erase(iter);
    }

    auto rect_viewport = QRect(0, 0, ui->verticalLayout_GL->geometry().width(),
        ui->verticalLayout_GL->geometry().height());

    QImage image;
    image.load("://icons/Square.png");
    //pCamItem->setImage(image);
    if( e_type == CameraOpenGLItem::PairOne )
    {
        rect_viewport = QRect(rect_viewport.left(), rect_viewport.top(),
                              rect_viewport.width()/3.0, rect_viewport.height());
    }
    else if( e_type == CameraOpenGLItem::PairTwo )
    {
        rect_viewport = QRect(rect_viewport.left() + rect_viewport.width()/3.0, rect_viewport.top(),
                              rect_viewport.width()/3.0, rect_viewport.height());
    }
    else if( e_type == CameraOpenGLItem::Result )
    {
        rect_viewport = QRect(rect_viewport.left() + rect_viewport.width()*2.0/3.0, rect_viewport.top(),
                              rect_viewport.width()/3.0, rect_viewport.height());
    }


    QGraphicsItem* pCamItem = new CameraOpenGLItem(Utility::e_Picture, rect_viewport, e_type);

    pCamItem->setPos(rect_viewport.left(), rect_viewport.top());
    //pCamItem->setRect(QRect(0, 0, rect_viewport.width(), rect_viewport.height()));


    CameraOpenGLItem* pCamItemGL = nullptr;
    if (e_type != CameraOpenGLItem::Result)
    {
        QFileDialog *fileDialog = new QFileDialog(this);
        fileDialog->setWindowTitle(tr("Open Image"));
        fileDialog->setDirectory(".");
        //fileDialog->setFilter(tr("Image Files(*.obj)"));
        if (fileDialog->exec() != QDialog::Accepted)
        {
            QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
            return;
        }

        QString path = fileDialog->selectedFiles()[0];
        //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);


        pcl::PolygonMesh mesh;
        pcl::io::loadOBJFile(path.toStdString(), mesh);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::fromPCLPointCloud2(mesh.cloud, *cloud);

        if (e_type == CameraOpenGLItem::PairOne)
        {
            *m_cloud1 = *cloud;
        }
        else if (e_type == CameraOpenGLItem::PairTwo)
        {
            *m_cloud2 = *cloud;
        }

        pCamItemGL = dynamic_cast<CameraOpenGLItem*>(pCamItem);
        std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> data = cloud->points;

        pCamItemGL->setPoints(data);

        if (e_type == CameraOpenGLItem::PairOne)
            m_cp1 = data;
        else
            m_cp2 = data;
    }
    else
    {
        //std::vector<pcl::PointXYZ, Eigen::aligned_allocator<pcl::PointXYZ>> allPoints;
        //allPoints = m_data[0];
        //allPoints.insert(allPoints.end(), m_data[1].begin(), m_data[1].end());
        pCamItemGL = dynamic_cast<CameraOpenGLItem*>(pCamItem);
        pCamItemGL->setAllPoints(m_cp1, m_cp2);
    }

    pCamItemGL->setDisplayType(e_type);

    m_Cam_items.insert(make_pair(e_type, pCamItemGL));

    m_scene->addItem(pCamItemGL);

    auto f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto iter = m_Cam_items.begin(); iter != m_Cam_items.end(); ++iter)
        iter->second->update();

    m_scene->update();
}

fpfhFeature::Ptr MainWindow::compute_fpfh_feature(pointcloud::Ptr input_cloud, pcl::search::KdTree<pcl::PointXYZ>::Ptr tree)
{
    //法向量
    pointnormal::Ptr point_normal(new pointnormal);
    pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> est_normal;
    est_normal.setInputCloud(input_cloud);
    est_normal.setSearchMethod(tree);
    est_normal.setKSearch(10);
    //norm_est.setRadiusSearch(0.05);
    //float normal_radius_ = 0.02f;
    //est_normal.setRadiusSearch(normal_radius_);
    est_normal.compute(*point_normal);
    //fpfh 估计
    fpfhFeature::Ptr fpfh(new fpfhFeature);
    //pcl::FPFHEstimation<pcl::PointXYZ,pcl::Normal,pcl::FPFHSignature33> est_target_fpfh;
    pcl::FPFHEstimation<pcl::PointXYZ, pcl::Normal, pcl::FPFHSignature33> est_fpfh; //FPFHEstimationOMP
    //est_fpfh.setNumberOfThreads(4); //指定4核计算
                                    // pcl::search::KdTree<pcl::PointXYZ>::Ptr tree4 (new pcl::search::KdTree<pcl::PointXYZ> ());
    est_fpfh.setInputCloud(input_cloud);
    est_fpfh.setInputNormals(point_normal);
    est_fpfh.setSearchMethod(tree);
    est_fpfh.setKSearch(10);

    //float feature_radius_ = 0.02f;
    //est_fpfh.setRadiusSearch(feature_radius_);

    est_fpfh.compute(*fpfh);

    return fpfh;
}

void MainWindow::on_coarse_btn_clicked()
{
    load3DFile(CameraOpenGLItem::Result);

    //Downsample and add pointcloud to array
    pcl::PointCloud<pcl::PointXYZ>::Ptr mv_downSampledPCs_1;
    float mv_voxelSideLength = 1.0;     //Size in meters used in the downsampling of input for inital alignment
    mv_downSampledPCs_1 = mf_voxelDownSamplePointCloud(m_cloud1, mv_voxelSideLength);

    //------------Target
    //Downsample and add pointcloud to array
    pcl::PointCloud<pcl::PointXYZ>::Ptr mv_downSampledPCs_2;
    mv_downSampledPCs_2 = mf_voxelDownSamplePointCloud(m_cloud2, mv_voxelSideLength);

    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>());

    fpfhFeature::Ptr source_fpfh = compute_fpfh_feature(mv_downSampledPCs_1, tree);
    fpfhFeature::Ptr target_fpfh = compute_fpfh_feature(mv_downSampledPCs_2, tree);

    // Initialize the parameters in the Sample Consensus Initial Alignment (SAC-IA) algorithm
    pcl::SampleConsensusInitialAlignment<pcl::PointXYZ, pcl::PointXYZ, pcl::FPFHSignature33> sac_ia;

    sac_ia.setMinSampleDistance(0.05f);
    sac_ia.setMaxCorrespondenceDistance(0.1f);
    sac_ia.setMaximumIterations(50);

    pointcloud::Ptr source = mv_downSampledPCs_1;
    pointcloud::Ptr target = mv_downSampledPCs_2;

    sac_ia.setInputSource(source);
    sac_ia.setSourceFeatures(source_fpfh);
    sac_ia.setInputTarget(target);
    sac_ia.setTargetFeatures(target_fpfh);
    pointcloud::Ptr align(new pointcloud);
    //sac_ia.setNumberOfSamples(20);
    //sac_ia.setCorrespondenceRandomness(6);
    //6
    //float max_correspondence_distance_ = 0.01f*0.01f;
    //float min_sample_distance_ = 0.05f;    
    //int nr_iterations_ = 50;
    //sac_ia.setMinSampleDistance(min_sample_distance_);
    //sac_ia.setMaxCorrespondenceDistance(max_correspondence_distance_);
    //sac_ia.setMaximumIterations(nr_iterations_);

    sac_ia.align(*align);
        
    //auto fitness_score = (float)sac_ia.getFitnessScore(max_correspondence_distance_);
    auto final_transformation = sac_ia.getFinalTransformation();

    displayTransform(final_transformation);

}

 template<typename T>
void MainWindow::displayTransform(const T& final_transformation)
{
    ui->label_matrix->clear();
    ui->label_matrix->update();

    auto pCamItem = m_Cam_items.find(CameraOpenGLItem::Result);
    if (pCamItem != m_Cam_items.end())
    {
        QMatrix4x4 matrix(final_transformation(0, 0), final_transformation(0, 1), final_transformation(0, 2), final_transformation(0, 3),
            final_transformation(1, 0), final_transformation(1, 1), final_transformation(1, 2), final_transformation(1, 3),
            final_transformation(2, 0), final_transformation(2, 1), final_transformation(2, 2), final_transformation(2, 3),
            final_transformation(3, 0), final_transformation(3, 1), final_transformation(3, 2), final_transformation(3, 3)
            );

        QString strMatrix;

        strMatrix = QString("%1").arg(matrix.toTransform().m11()) + " | "
            + QString("%1").arg(matrix.toTransform().m12()) + " | "
            + QString("%1").arg(matrix.toTransform().m13()) + " | \n"
            + QString("%1").arg(matrix.toTransform().m21()) + " | "
            + QString("%1").arg(matrix.toTransform().m22()) + " | "
            + QString("%1").arg(matrix.toTransform().m23()) + " | \n"
            + QString("%1").arg(matrix.toTransform().m31()) + " | "
            + QString("%1").arg(matrix.toTransform().m32()) + " | "
            + QString("%1").arg(matrix.toTransform().m33()) + " | \n";

        ui->label_matrix->setWordWrap(true);
        ui->label_matrix->setText(strMatrix);
        ui->label_matrix->update();

        ui->label_matrix->parentWidget()->update();

        auto pCamItemGL = dynamic_cast<CameraOpenGLItem*>(pCamItem->second);
        if(pCamItemGL)
            pCamItemGL->setTransform(matrix);
    }
}

void MainWindow::on_fine_btn_clicked()
{
    load3DFile(CameraOpenGLItem::Result);
    pointcloud::Ptr source = m_cloud2;
    pointcloud::Ptr target = m_cloud1;

    pcl::PointCloud<pcl::PointXYZ>::Ptr output(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
    //设置对应点对之间的最大距离（此值对配准结果影响较大)
    icp.setMaxCorrespondenceDistance(1000);
    //设置两次变化矩阵之间的差值（一般设置为1e-10即可）
    icp.setTransformationEpsilon(1e-15);
    //设置收敛条件是均方误差和小于阈值， 停止迭代；
    icp.setEuclideanFitnessEpsilon(0.0000001);
    //最大迭代次数，icp是一个迭代的方法，最多迭代这些次（若结合可视化并逐次显示，可将次数设置为1）；
    icp.setMaximumIterations (10000);

    //icp.setTransformationEstimation();

    icp.setInputSource (source);
    icp.setInputTarget (target);
    icp.align (*output);

    auto final_transformation = icp.getFinalTransformation();
    displayTransform(final_transformation);
}

void MainWindow::on_fusion_btn_clicked()
{
    load3DFile(CameraOpenGLItem::Result);


	//Remove outliers and store reference to denoised Pointcloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_originalRotatedPCs_1 = m_cloud1;
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_originalRotatedDenoisedPCs_1 = mf_outlierRemovalPC(mv_originalRotatedPCs_1);

	//Downsample and add pointcloud to array
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_downSampledPCs_1;
	float mv_voxelSideLength = 0.015;     //Size in meters used in the downsampling of input for inital alignment
	mv_downSampledPCs_1 = mf_voxelDownSamplePointCloud(mv_originalRotatedDenoisedPCs_1, mv_voxelSideLength);

	//Compute normals from downsampled pointcloud for correspondence estimation
	pcl::PointCloud<pcl::Normal>::Ptr mv_downSampledNormals_1;
	float mv_normalRadiusSearch = 0.05;    //Normal radius search for correspondence matching
	mv_downSampledNormals_1 = mf_computeNormals(mv_downSampledPCs_1, mv_normalRadiusSearch);

	//------------Target

	//Remove outliers and store reference to denoised Pointcloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_originalRotatedPCs_2 = m_cloud2;
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_originalRotatedDenoisedPCs_2 = mf_outlierRemovalPC(mv_originalRotatedPCs_2);

	//Downsample and add pointcloud to array
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_downSampledPCs_2;
	mv_downSampledPCs_2 = mf_voxelDownSamplePointCloud(mv_originalRotatedDenoisedPCs_2, mv_voxelSideLength);

	//Compute normals from downsampled pointcloud for correspondence estimation
	pcl::PointCloud<pcl::Normal>::Ptr mv_downSampledNormals_2;
	mv_downSampledNormals_2 = mf_computeNormals(mv_downSampledPCs_2, mv_normalRadiusSearch);

	//------------------

	//Compute correspondences between new pointcloud and last aligned pointcloud
	pcl::CorrespondencesPtr mv_downsampledCorrespondences;
	double mv_SVD_MaxDistance = 0.15;     //Max distance in meters between two points to find correspondances for SVD
	mv_downsampledCorrespondences = mf_estimateCorrespondences(
		mv_downSampledPCs_1, mv_downSampledPCs_2, mv_downSampledNormals_1, mv_downSampledNormals_2, mv_SVD_MaxDistance);

	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_alignedDownSampledPCs_1 = mv_downSampledPCs_1;

	//Align downsampled pointclouds using SVD and get transform to apply on original later
	Eigen::Matrix4f SVDtransform;
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_alignedDownSampledPCs_2 = mf_SVDInitialAlignment(
		mv_downSampledPCs_1,        //Previous DownsampledAlignedPointcloud
		mv_downSampledPCs_2,        //New DownsampledPointclout to be aligned
		mv_downsampledCorrespondences, SVDtransform);

	//Store rough alignment transform for use later
	Eigen::Matrix4f mv_transformationMatrices_1 = SVDtransform;

	//Perform second step of initial alignment with ICP
	Eigen::Matrix4f ICPtransform;
	float mv_ICP_MaxCorrespondenceDistance = 0.05; //Max distance in meters between two points to find correspondances for ICP
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_alignedDownSampledPCs_3 = mf_iterativeClosestPointFinalAlignment<pcl::PointXYZ>(
		mv_alignedDownSampledPCs_1,
		mv_alignedDownSampledPCs_2,
		mv_ICP_MaxCorrespondenceDistance, ICPtransform);

	//Add both transforms together to make initial alignment transform
	Eigen::Matrix4f mv_transformationMatrices_2 = ICPtransform * mv_transformationMatrices_1;

	//Apply compound transformation to roughly align original pointcloud to previous one
	pcl::PointCloud<pcl::PointXYZ>::Ptr mv_alignedOriginalPCs_2;
	pcl::PointCloud<pcl::PointXYZ>::Ptr alignedOriginal(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*mv_originalRotatedDenoisedPCs_2, *alignedOriginal, mv_transformationMatrices_2);
	mv_alignedOriginalPCs_2 = alignedOriginal;

	//Recompute Normals once the Downsampled pointcloud has been aligned for use later
	//mv_downSampledNormals_1 = mf_computeNormals( mv_alignedDownSampledPCs_2, mv_normalRadiusSearch );


	auto final_transformation = mv_transformationMatrices_2;
	displayTransform(final_transformation);

	m_alignedPCs = m_cloud1;
	m_alignedPCs->insert(m_alignedPCs->end(), mv_alignedOriginalPCs_2->begin(), mv_alignedOriginalPCs_2->end());
}

void MainWindow::on_global_btn_clicked()
{

}

/////////////////////////////////////////////////////

pcl::PointCloud<pcl::PointXYZ>::Ptr MainWindow::mf_outlierRemovalPC(
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in,
    const float meanK, const float std_dev )
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sorfilter(true);
    sorfilter.setInputCloud(cloud_in);
    sorfilter.setMeanK(meanK);
    sorfilter.setStddevMulThresh(std_dev);
    sorfilter.filter(*cloud_out);

    return cloud_out;

    //Use RadiusOutlierRemoval to remove the point which is far away to others  
    //pcl::RadiusOutlierRemoval<pcl::PointXYZ> outrem;
    //outrem.setInputCloud(pcl_vg_cloud_ptr);
    //outrem.setRadiusSearch(0.5);
    //outrem.setMinNeighborsInRadius(3);
    //outrem.filter(*pcl_ror_cloud_ptr);
}

/////////////////////////////////////////////////////

pcl::PointCloud<pcl::PointXYZ>::Ptr MainWindow::mf_voxelDownSamplePointCloud(
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in, const float &voxelSideLength)
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr downSampledPointCloud(new pcl::PointCloud<pcl::PointXYZ>);

    pcl::VoxelGrid<pcl::PointXYZ> vg;
    vg.setLeafSize(voxelSideLength, voxelSideLength, voxelSideLength);

    vg.setInputCloud(cloud_in);
    vg.filter(*downSampledPointCloud);

    return downSampledPointCloud;
}

/////////////////////////////////////////////////////
pcl::PointCloud<pcl::Normal>::Ptr MainWindow::mf_computeNormals(
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_in, const float &searchRadius )
{
    NormalEstimation<PointXYZ, Normal> ne;
    PointCloud<Normal>::Ptr normals(new PointCloud<Normal>);
    search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>);

    tree->setInputCloud(cloud_in);
    ne.setInputCloud(cloud_in);
    ne.setSearchMethod(tree);
    ne.setKSearch(20);
    ne.compute(*normals);       //calculate normal vector


    ////Create output pointer
    //pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);

    ////Instantiate Normal Estimator
    //pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> norm_est;

    ////Configure estimator
    //norm_est.setInputCloud(cloud_in);
    //norm_est.setRadiusSearch(searchRadius);

    //norm_est.compute(*normals);

    return normals;
}

/////////////////////////////////////////////////////
pcl::CorrespondencesPtr MainWindow::mf_estimateCorrespondences(
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud1,
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud2,
    const pcl::PointCloud<pcl::Normal>::Ptr &normals1,
    const pcl::PointCloud<pcl::Normal>::Ptr &normals2,
    const double &max_distance)
{
    pcl::registration::CorrespondenceEstimationBackProjection<pcl::PointXYZ, pcl::PointXYZ, pcl::Normal> corr_est;

    corr_est.setInputSource(cloud1);
    corr_est.setSourceNormals(normals1);

    corr_est.setInputTarget(cloud2);
    corr_est.setTargetNormals(normals2);


    pcl::CorrespondencesPtr all_correspondences(new pcl::Correspondences());
    corr_est.determineReciprocalCorrespondences(*all_correspondences);

    pcl::registration::CorrespondenceRejectorSampleConsensus<pcl::PointXYZ> rejector;

    rejector.setInputSource(cloud1);
    rejector.setInputTarget(cloud2);
    rejector.setInputCorrespondences(all_correspondences);

    //float rej_sac_max_dist = 0.1;
    float rej_sac_max_dist = 0.1;
    rejector.setInlierThreshold(rej_sac_max_dist);
    rejector.setSaveInliers(false);
    rejector.setRefineModel(false);

    //Add source and target pointcloud data to rejector?
    pcl::CorrespondencesPtr remaining_correspondences(new pcl::Correspondences());
    rejector.getCorrespondences(*remaining_correspondences);

    return remaining_correspondences;
}


/////////////////////////////////////////////////////
pcl::PointCloud<pcl::PointXYZ>::Ptr MainWindow::mf_SVDInitialAlignment(
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &source,
    const pcl::PointCloud<pcl::PointXYZ>::Ptr &target,
    pcl::CorrespondencesPtr correspondences,
    Eigen::Matrix4f &transformation_matrix)
{
    transformation_matrix = Eigen::Matrix4f();

    //Estimate transformation that converts cloud1 -> cloud2
    pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ> TESVD;
    TESVD.estimateRigidTransformation(*source, *target, *correspondences, transformation_matrix);

    pcl::PointCloud<pcl::PointXYZ>::Ptr alignedSource(new pcl::PointCloud<pcl::PointXYZ>());
    pcl::transformPointCloud(*source, *alignedSource, transformation_matrix);

    return alignedSource;
}

/////////////////////////////////////////////////////
template <typename PointT>
boost::shared_ptr<pcl::PointCloud<PointT>>MainWindow::mf_iterativeClosestPointFinalAlignment(
    const boost::shared_ptr<pcl::PointCloud<PointT>> &source,
    const boost::shared_ptr<pcl::PointCloud<PointT>> &target,
    const float &maxCorrespondenceDistance,
    Eigen::Matrix4f &icpTransformation)
{
    pcl::IterativeClosestPoint<PointT, PointT> icp;

    icp.setMaxCorrespondenceDistance(maxCorrespondenceDistance);

    //设置对应点对之间的最大距离（此值对配准结果影响较大)
    icp.setMaxCorrespondenceDistance(100);
    //设置两次变化矩阵之间的差值（一般设置为1e-10即可）
    icp.setTransformationEpsilon(1e-15);
    //设置收敛条件是均方误差和小于阈值， 停止迭代；
    icp.setEuclideanFitnessEpsilon(1e-15);
    //最大迭代次数，icp是一个迭代的方法，最多迭代这些次（若结合可视化并逐次显示，可将次数设置为1）；
    icp.setMaximumIterations(50);

    icp.setInputCloud(source);
    icp.setInputTarget(target);

    pcl::PointCloud<PointT>::Ptr alignedSource(new pcl::PointCloud<PointT>);
    icp.align(*alignedSource);

    icpTransformation = icp.getFinalTransformation();
    return alignedSource;
}

void MainWindow::mf_NormalEstimation(const pcl::PointCloud<pcl::PointXYZ>::Ptr &mv_PointCloudInput,
    pcl::PointCloud<pcl::PointNormal>::Ptr &mv_PointNormalOutput)
{
    NormalEstimation<PointXYZ, Normal> ne;
    PointCloud<Normal>::Ptr normals(new PointCloud<Normal>);
    search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>);

    tree->setInputCloud(mv_PointCloudInput);
    ne.setInputCloud(mv_PointCloudInput);
    ne.setSearchMethod(tree);
    ne.setKSearch(20);
    ne.compute(*normals);       //calculate normal vector

    PointCloud<PointNormal>::Ptr cloud_with_normals(new PointCloud<PointNormal>);
    //combine normal vectors with points
    concatenateFields(*mv_PointCloudInput, *normals, *mv_PointNormalOutput);
}

void MainWindow::mf_PoissonMeshes(pcl::PointCloud<pcl::PointXYZ>::Ptr mv_PointCloudInput, 
    pcl::PolygonMesh::Ptr &mv_MeshesOutput)
{
    //Normal Estimation function
    PointCloud<PointNormal>::Ptr cloud_with_normals(new PointCloud<PointNormal>());
    mf_NormalEstimation(mv_PointCloudInput, cloud_with_normals);

    //Poisson Function

    search::KdTree<PointNormal>::Ptr tree2(new search::KdTree<PointNormal>);
    tree2->setInputCloud(cloud_with_normals);

    Poisson<PointNormal> poisson_reconstruct;
    //poisson_reconstruct.setConfidence(false);
    //poisson_reconstruct.setDegree(2);
    //poisson_reconstruct.setDepth(9);
    //poisson_reconstruct.setIsoDivide(8);
    //poisson_reconstruct.setManifold(false);
    //poisson_reconstruct.setSolverDivide(8);
    //poisson_reconstruct.setSamplesPerNode(3.0);
    //poisson_reconstruct.setScale(1.25);
    poisson_reconstruct.setSearchMethod(tree2);
    poisson_reconstruct.setInputCloud(cloud_with_normals);
    //PolygonMesh mesh;
    poisson_reconstruct.reconstruct(*mv_MeshesOutput);
}


void MainWindow::on_checkBoxMesh_clicked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

    auto pCamItem = m_Cam_items.find(CameraOpenGLItem::Result);
    if (pCamItem != m_Cam_items.end())
    {
        CameraOpenGLItem* pCamItemGL = dynamic_cast<CameraOpenGLItem*>(pCamItem->second);
        pcl::PolygonMesh::Ptr pMesh(new pcl::PolygonMesh());
        mf_PoissonMeshes(m_alignedPCs, pMesh);

        pCamItemGL->setMesh(pMesh, checked);
    }
}
//
//const int DataSize = 100;
//const int BufferSize = 100;
//char buffer[BufferSize];
//
////与信号量不同的程序  
//QWaitCondition bufferIsNotFull; //QWaitCondition允许在一定条件下触发其它多个线程  
//QWaitCondition bufferIsNotEmpty;
//QMutex mutex;
//int usedSpace = 0;//不是信号量，而是用来代表在缓冲器中存在多少个“用过的”字节  
//
//                  //生产者  
//class Producer : public QThread
//{
//public:
//    void run();
//};
//
//void Producer::run()
//{
//    for (int i = 0; i < DataSize; ++i) {
//        mutex.lock();//锁定：保护对usedSpace变量的访问  
//                     //如果缓冲区都是用过的字节，那么就等待  
//        while (usedSpace == BufferSize)
//            bufferIsNotFull.wait(&mutex);//wait的过程：解锁－>阻塞当前线程－>满足条件－>锁定－>返回  
//        buffer[i % BufferSize] = "ACGT"[uint(rand()) % 4];
//        std::cerr << "P";//代表一次生产者线程的运行  
//        ++usedSpace;
//        bufferIsNotEmpty.wakeAll();//Wakes all threads waiting on the wait condition.  
//        mutex.unlock();//解锁  
//    }
//}
//
////消费者  
//class Consumer : public QThread
//{
//public:
//    void run();
//};
//
//void Consumer::run()
//{
//    for (int i = 0; i < DataSize; ++i) {
//        mutex.lock();//  
//                     //如果缓冲区里“用过的”字节数目为0，则等待  
//        while (usedSpace == 0)
//            bufferIsNotEmpty.wait(&mutex);
//        //std::cerr << buffer[i % BufferSize];  
//        char Cons = buffer[i % BufferSize];
//        std::cerr << "c";
//        --usedSpace;
//        bufferIsNotFull.wakeAll();
//        mutex.unlock();
//    }
//    std::cerr << std::endl;
//}
//
//int MainWindow::test()
//{
//    Producer producer;
//    Consumer consumer;
//    producer.start();
//    consumer.start();
//    producer.wait();
//    consumer.wait();
//    return 0;
//}