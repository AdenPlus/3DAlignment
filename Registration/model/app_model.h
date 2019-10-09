#pragma once

#include "def.h"
#include "singleton.h"
#include <unordered_map>
#include <QPair>
#include <QSet>
#include <QRect>
#include <QImage>
#include <QTransform>
#include "utility.h"

/**********************************************************************************************************************
@brief:
***********************************************************************************************************************/
class QGraphicsScene;
class CameraSetting;
class AppModel : public QObject, public Singleton<AppModel>
{
    static const int AUTO_HIDE_INTERVAL = 3;

Q_OBJECT
    ADD_FIELD_ALL(bool, DisplayMode, false) // pip mode is true, split mode is false
    ADD_FIELD_ALL(Utility::e_WindowMode, WindowMode, Utility::e_Normal) //normal window size and mimi window is false
    ADD_FIELD_ALL(bool, PresentMode, false)
    ADD_FIELD_ALL(QGraphicsScene*, PresentationScene, nullptr)
    ADD_FIELD_ALL(bool, AnnotationMode, false)
    ADD_FIELD_ALL(bool, DownCamFreezeMode, false)
    ADD_FIELD_ALL(bool, WebCamFreezeMode, false)
    ADD_FIELD_ALL(bool, DesktopFreezeMode, false)
    ADD_FIELD_ALL(bool, MatFreezeMode, false)
    ADD_FIELD_ALL(QTransform, PresentMatrix, QTransform())
    ADD_FIELD_ALL(int, Brightness, 0)
    ADD_FIELD_ALL(int, Contrast, 0)
    ADD_FIELD_ALL(Utility::e_SourceMode, SourceMode, Utility::e_Stream_Mode)
    ADD_FIELD_ALL(Utility::EXCHANGE_TYPE, ExchangedType, Utility::EXCHANGE_TYPE())
    ADD_FIELD_ALL(QVector<int>, CameraIndex, QVector<int>())
    ADD_FIELD_ALL(bool, RecordMode, false)
    ADD_FIELD_ALL(bool, LivePreviewEnabled, false)
    ADD_FIELD_ALL(Utility::e_ResolutionType, DownCamResType, Utility::e_High_Res)
    ADD_FIELD_ALL(Utility::e_ResolutionType, WebCamResType, Utility::e_High_Res)
    ADD_FIELD_ALL(int, AutoHideTime, AUTO_HIDE_INTERVAL)

    ADD_FIELD_ALL(bool, DownCamPreviewEnabled, false)
    ADD_FIELD_ALL(bool, WebCamPreviewEnabled, false)
    ADD_FIELD_ALL(bool, DesktopPreviewEnabled, false)
    ADD_FIELD_ALL(bool, MatPreviewEnabled, false)

    ADD_FIELD_ALL(Utility::DesktopRectImage, LastDesktopShot, Utility::DesktopRectImage())

public:
    Utility::STREAMS_INFO getStreamsInfo() { return m_streamsInfo; };
    void setStreamsInfo(Utility::e_StreamType streamType, const QSize& imageSize, bool add);
    void setStreamsInfo(const Utility::STREAMS_INFO& streamsInfo);

    Utility::ITEMS_RECT getItemsRect() { return m_itemsRect; };
    void setItemsRect(Utility::e_StreamType streamType, const QRect& itemRect);
    Utility::ITEMS_RATIO getStreamsRatio() { return m_streamRatios; };

    QPair<QSize, QSize> getDownCamResolutions(Utility::e_ResolutionType type);
    QSize AppModel::getWebCamResolutions(Utility::e_ResolutionType type);

signals:
    void sigStreamsInfoChanged(const Utility::STREAMS_INFO& streamsInfo);
    void sigItemRectChanged(Utility::e_StreamType streamType, const QRect& itemRect);
    void sigCameraInUse(Utility::e_StreamType streamType);

protected:
    AppModel();
    friend class Singleton<AppModel>;

private:
    Utility::STREAMS_INFO m_streamsInfo;
    Utility::ITEMS_RECT m_itemsRect; //based on the layout window 1260 * 710
    Utility::ITEMS_RATIO m_streamRatios;
    QSharedPointer<CameraSetting> m_cameraSetting;
};
