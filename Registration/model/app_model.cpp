#include "stdafx.h"
#include "app_model.h"
#include <QGraphicsScene>

static const QMap<Utility::e_ResolutionType, QPair<QSize, QSize>> HIGHRES_CAMINFO =
{
    { Utility::e_High_Res, QPair<QSize, QSize>(QSize(4416,3312), QSize(4200,2800)) },
    { Utility::e_Normal_Res, QPair<QSize, QSize>(QSize(2208,1656), QSize(1920, 1280)) }
};

static const QMap<Utility::e_ResolutionType, QSize> WEB_CAMINFO =
{
    { Utility::e_High_Res, QSize(1920,1080) },
    { Utility::e_Normal_Res, QSize(1280,720) }
};

using namespace std;

AppModel::AppModel()
{
    auto settings = Utility::applicationSettings();
    m_LivePreviewEnabled = settings->value("Live_Preview_Enable", false).toBool();
    if (!m_LivePreviewEnabled)
    {
        settings->setValue("Live_Preview_Enable", m_LivePreviewEnabled);
    }
    else
    {
        m_DownCamPreviewEnabled = true;
        m_WebCamPreviewEnabled = true;
        m_DesktopPreviewEnabled = true;
        m_MatPreviewEnabled = true;
    }

    m_DownCamResType = Utility::e_ResolutionType(settings->value("DownCam_Resolution", 0).toInt());
    if (m_DownCamResType == Utility::e_High_Res)
        settings->setValue("DownCam_Resolution", 0);

    m_WebCamResType = Utility::e_ResolutionType(settings->value("WebCam_Resolution", 0).toInt());
    if (m_WebCamResType == Utility::e_High_Res)
        settings->setValue("WebCam_Resolution", 0);

    m_AutoHideTime = settings->value("Auto_Hide_Time", AUTO_HIDE_INTERVAL).toInt();
    if (m_AutoHideTime == AUTO_HIDE_INTERVAL)
        settings->setValue("Auto_Hide_Time", AUTO_HIDE_INTERVAL);
}

void AppModel::setStreamsInfo(Utility::e_StreamType streamType, const QSize& imageSize, bool add )
{
    if(add)
    {
        if (m_streamsInfo.find(streamType) == m_streamsInfo.end())
        {
            m_streamsInfo.insert(make_pair(streamType, imageSize));
            m_streamRatios.insert(make_pair(streamType, 1.0f * imageSize.height() / imageSize.width()));
        }
    }
    else
    {
        auto iter = m_streamsInfo.find(streamType);
        if (iter != m_streamsInfo.end())
        {
            m_streamsInfo.erase(iter);
        }

        auto iter_ratio = m_streamRatios.find(streamType);
        if (iter_ratio != m_streamRatios.end())
        {
            m_streamRatios.erase(iter_ratio);
        }
    }

    emit sigStreamsInfoChanged(m_streamsInfo);
}

void AppModel::setStreamsInfo(const Utility::STREAMS_INFO& streamsInfo)
{
    m_streamsInfo = streamsInfo;

    emit sigStreamsInfoChanged(m_streamsInfo);
}

void AppModel::setItemsRect(Utility::e_StreamType streamType, const QRect& itemRect)
{
    if (m_itemsRect.find(streamType) == m_itemsRect.end())
        m_itemsRect.insert(make_pair(streamType, itemRect));
    else
        m_itemsRect[streamType] = itemRect;

    emit sigItemRectChanged(streamType, itemRect);
}

QPair<QSize, QSize> AppModel::getDownCamResolutions(Utility::e_ResolutionType type)
{
    if (HIGHRES_CAMINFO.contains(type))
        return HIGHRES_CAMINFO[type];

    return QPair<QSize, QSize>();
}

QSize AppModel::getWebCamResolutions(Utility::e_ResolutionType type)
{
    if (WEB_CAMINFO.contains(type))
        return WEB_CAMINFO[type];

    return QSize();
}
