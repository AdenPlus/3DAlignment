#ifndef UTILITY_H
#define UTILITY_H

#include <Windows.h>
#include <QObject>
#include <QImage>
#include <QScreen>
#include <QRect>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QWidget>
#include <QPushButton>
#include <QPoint>
#include <QSize>
#include <unordered_map>
#include <QSettings>


class Utility : public QObject
{
Q_OBJECT
public:
    Utility(QObject *parent=NULL);

    enum e_StreamType
    {
        e_DownCam = 0,
        e_WebCam,
        e_Desktop,
        e_Mat,
        e_Picture
    };
    Q_ENUM(e_StreamType)

    enum e_ScreenType
    {
        e_MonitorScreen = 0,
        e_MatScreen,
        e_PresentScreen,
        e_CompositeScreen
    };
    Q_ENUM(e_ScreenType)

    enum RenderSourceType
    {
        e_OpenCVMat = 0,
        e_QImage
    };
    Q_ENUM(RenderSourceType)

    enum e_WindowMode
    {
        e_Normal,
        e_Mini,
        e_Close
    };

    Q_ENUM(e_WindowMode)

    enum e_SourceMode
    {
        e_Stream_Mode=0,
        e_Picture_Mode
    };

    Q_ENUM(e_SourceMode)

    enum e_ResolutionType
    {
        e_High_Res = 0,
        e_Normal_Res,
    };
    Q_ENUM(e_ResolutionType)

    typedef std::unordered_map<Utility::e_StreamType, QSize> STREAMS_INFO;
    typedef std::unordered_map<Utility::e_StreamType, QRect> ITEMS_RECT;
    typedef std::unordered_map<Utility::e_StreamType, float> ITEMS_RATIO;
    typedef QPair<Utility::e_StreamType, Utility::e_StreamType> EXCHANGE_TYPE;
    typedef QVector<QPair<QRect, QImage>> DesktopRectImage;

public:
    static void initialization();

    static QScreen* findScreen(const e_ScreenType& type, int* index = nullptr);

    static QRect findScreenGeometry(const e_ScreenType& type);

    static char* QString2Char(const QString& strInput);
    static int QString2UTF8(const QString& strInput, char* UTF8code);

    static bool fileExists(const QString& path);
    static bool deleteFile(const QString& path);
    static QString renameFile(const QString& path);

    static void setButtonState(QPushButton* btn, bool enabled, const QString& iconName);

    static Utility::ITEMS_RECT calcStreamLayouts(bool pipMode, const QSize& fullSize, const ITEMS_RATIO& ratios);

    static int getNormalTextPixel(const QString& text, int fontSize = 14, int fontWeight = 50);

    static bool isProcessRunning();

    static QSharedPointer<QSettings> applicationSettings();

private:
    static void setHardwareIds(QStringList monitorHardwareIds, QStringList matHardwareIds);
    static QStringList monitorHardwareIds() { return m_monitorHardwareIds; }
    static QStringList matHardwareIds() { return m_matHardwareIds; }
    static QMap<QString, QString> getScreenNames();

    DWORD getProcessIdByName(const QString& processName);

private:
    static QStringList m_monitorHardwareIds;
    static QStringList m_matHardwareIds;
};

Q_DECLARE_METATYPE(Utility::e_StreamType)
Q_DECLARE_METATYPE(Utility::e_ScreenType)
Q_DECLARE_METATYPE(QVector<Utility::e_ScreenType>)
Q_DECLARE_METATYPE(QWidget*)

#endif // UTILITY_H
