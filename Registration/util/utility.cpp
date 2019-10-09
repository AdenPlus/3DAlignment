#include "stdafx.h"
#include "utility.h"
#include <QHash>
#include <QDebug>
#include <QGuiApplication>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <psapi.h>
#include <tlhelp32.h>
#include <QProcessEnvironment>

#include "ogl_graphicsview.h"
#include "app_model.h"

const QString MONITOR = "monitor";
const QString MAT = "mat";
const QString PRESENT = "present";

QStringList Utility::m_matHardwareIds = { "HWP4239" };
QStringList Utility::m_monitorHardwareIds = { "HWP4627", "HWP425D" };

static QHash<Utility::e_ScreenType, QString> ScreenTypeTranslationTable{
    { Utility::e_MonitorScreen, MONITOR },
    { Utility::e_MatScreen, MAT },
    { Utility::e_PresentScreen, PRESENT }};

using namespace std;

void Utility::initialization()
{
    static struct Initialize
    {
        Initialize()
        {
            qRegisterMetaType<Utility::e_StreamType>();
            qRegisterMetaType<Utility::e_ScreenType>();
            qRegisterMetaType<QVector<Utility::e_ScreenType>>();
            qRegisterMetaType<QWidget*>();
            qRegisterMetaType<OGLGraphicsView*>();
        }
    } initialize;
}

QScreen* Utility::findScreen(const e_ScreenType& type, int* index)
{
    QScreen* result = nullptr;
    QMap<QString, QString> map = getScreenNames();

    qInfo() << "The monitor screen name is" << map[MONITOR];
    qInfo() << "The mat screen name is" << map[MAT];
    qInfo() << "The present screen name is" << map[PRESENT];

    QList<QScreen*> screens = QGuiApplication::screens();

    int i = 0;
    QString key = ScreenTypeTranslationTable[type];
    foreach(auto screen, screens)
    {
        if (map.contains(key))
        {
            QString xxx = screen->name();
            if (screen->name() == map[key])
            {
                if (index)
                {
                    *index = i;
                }
                result = screen;
                return result;
            }
        }
        i++;
    }

    result = screens[0];
    return result;
}

void Utility::setHardwareIds(QStringList monitorHardwareIds, QStringList matHardwareIds) {
    m_monitorHardwareIds = monitorHardwareIds;
    m_matHardwareIds = matHardwareIds;
}

//[hardware]
//.mat_ids = HWP4239
//.monitor_ids = HWP4627, HWP425D

QRect Utility::findScreenGeometry(const e_ScreenType& type)
{
#ifdef Q_OS_WIN
    DISPLAY_DEVICE dd;

    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);

    QRect last_geometry;
    for (DWORD i = 0;; i++) 
    {
        if (EnumDisplayDevices(NULL, i, &dd, 0) == 0) 
            break;

        DISPLAY_DEVICE monInfo;
        monInfo.cb = sizeof(monInfo);

        for (DWORD j = 0; EnumDisplayDevices(dd.DeviceName, j, &monInfo, 0); j++)  // query all monitors on the adaptor
        {
            auto deviceId = QString::fromWCharArray(monInfo.DeviceID);
            auto deviceName = QString::fromWCharArray(dd.DeviceName);

            qDebug() << "Found device hardware ID" << deviceId << "as" << deviceName;

            DEVMODE defaultMode;

            ZeroMemory(&defaultMode, sizeof(DEVMODE));
            defaultMode.dmSize = sizeof(DEVMODE);

            if (!EnumDisplaySettings(dd.DeviceName, ENUM_REGISTRY_SETTINGS, &defaultMode))
            {
                qCritical() << "Can not enumerate the monitor setting!";
                break;
            }

            QRect geometry = QRect(defaultMode.dmPosition.x, defaultMode.dmPosition.y,
                defaultMode.dmPelsWidth, defaultMode.dmPelsHeight);
            last_geometry = geometry;

            bool bPresent = true;
            foreach(auto hardwareId, m_monitorHardwareIds) 
            {
                if (deviceId.contains(hardwareId)) 
                {
                    bPresent = false;
                    if (type == e_ScreenType::e_MonitorScreen)
                    {
                        qInfo() << "Find Monitor screen, the geometry is" << geometry;
                        return geometry;
                    }
                    break;
                }
            }

            foreach(auto hardwareId, m_matHardwareIds)
            {
                if (deviceId.contains(hardwareId)) 
                {
                    bPresent = false;
                    if (type == e_ScreenType::e_MatScreen)
                    {
                        qInfo() << "Find Mat screen, the geometry is" << geometry;
                        return geometry;
                    }
                    break;
                }
            }

            if (type == e_ScreenType::e_PresentScreen && bPresent)
            {
                qInfo() << "Find Present screen, the geometry is" << geometry;
                return geometry;
            }
        }
    }

#elif
#error Not implemented for this OS
#endif

    return last_geometry;
}

QMap<QString, QString> Utility::getScreenNames()
{
    QMap<QString, QString> screenMap;
#ifdef Q_OS_WIN
    DISPLAY_DEVICE dd;

    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);

    for (DWORD i = 0;; i++) {
        if (EnumDisplayDevices(NULL, i, &dd, 0) == 0) {
            break;
        }

        DISPLAY_DEVICE monInfo;
        monInfo.cb = sizeof(monInfo);

        for (DWORD j = 0; EnumDisplayDevices(dd.DeviceName, j, &monInfo, 0); j++)  // query all monitors on the adaptor
        {
            auto deviceId = QString::fromWCharArray(monInfo.DeviceID);
            auto deviceName = QString::fromWCharArray(dd.DeviceName);

            qDebug() << "Found device hardware ID" << deviceId << "as" << deviceName;

            foreach(auto hardwareId, m_monitorHardwareIds) {
                if (deviceId.contains(hardwareId)) {
                    screenMap.insert(MONITOR, deviceName);
                    break;
                }
            }

            foreach(auto hardwareId, m_matHardwareIds) {
                if (deviceId.contains(hardwareId)) {
                    screenMap.insert(MAT, deviceName);
                    break;
                }
            }

            if (screenMap.contains(MONITOR) && deviceName != screenMap[MONITOR] &&
                screenMap.contains(MAT) && deviceName != screenMap[MAT]) {
                screenMap.insert(PRESENT, deviceName);
            }
        }
    }

#elif
#error Not implemented for this OS
#endif

    return screenMap;
}

char* Utility::QString2Char(const QString& strInput)
{
    char* strOutput = 0;
    QByteArray ba = strInput.toLatin1(); // must
    strOutput = ba.data();
    return strOutput;
}

int Utility::QString2UTF8(const QString& strInput, char* UTF8code)
{
    char* pszCode = QString2Char(strInput);

    WCHAR Unicode[100] = { 0, };
    char utf8[100] = { 0, };

    // read char Lenth
    int nUnicodeSize = MultiByteToWideChar(CP_ACP, 0, pszCode, strlen(pszCode), Unicode, sizeof(Unicode));
    memset(UTF8code, 0, nUnicodeSize + 1);
    // read UTF-8 Lenth
    int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, Unicode, nUnicodeSize, UTF8code, sizeof(Unicode), NULL, NULL);

    // convert to UTF-8 
    MultiByteToWideChar(CP_UTF8, 0, utf8, nUTF8codeSize, Unicode, sizeof(Unicode));
    UTF8code[nUTF8codeSize + 1] = '\0';
    return nUTF8codeSize;
}

bool Utility::fileExists(const QString& path)
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

QString Utility::renameFile(const QString& path)
{
    QString newFileName = path;
    QFileInfo file(path);
    QString baseName = file.baseName();
    QString extName = file.suffix();

    const QString movieFolder = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    int n = 0;
    while (fileExists(newFileName))
    {
        n++;
        QString nstring = QString::number(n);
        newFileName = movieFolder + QDir::separator() + baseName + nstring + "." + extName;
    }

    return newFileName;
}

bool Utility::deleteFile(const QString& path)
{
    QFile file(path);
    return file.remove();
}

void Utility::setButtonState(QPushButton* btn, bool enabled, const QString& iconName)
{
    QString styleSheet = " \
                         QPushButton { \
            image: url(://icons/icon-"+iconName+"-norm.png); \
            background-color: transparent; \
            border: none; \
        } \
        QPushButton:hover { \
            image: url(://icons/icon-"+iconName+"-hover.png); \
         } \
        QPushButton:disabled { \
             image: url(://icons/icon-"+iconName+"-disable.png); \
        } \
        QPushButton:pressed { \
            image:  url(://icons/icon-"+iconName+"-press.png); \
        } \
        QPushButton:checked { \
            image:  url(://icons/icon-"+iconName+"-press.png); \
        } \
    ";
    btn->setStyleSheet(styleSheet);
    btn->setEnabled(enabled);
}

int Utility::getNormalTextPixel(const QString& text, int fontSize, int fontWeight)
{
    QFont font;
    font.setFamily("Segoe UI");
    font.setPixelSize(fontSize);
    font.setWeight(fontWeight);
    font.setStyle(QFont::StyleNormal);

    QFontMetrics fm(font);

    return fm.width(text);
}

Utility::ITEMS_RECT Utility::calcStreamLayouts(bool pipMode, const QSize& fullSize, const ITEMS_RATIO& ratios)
{
    ITEMS_RECT itemSizePos;
    auto streamsRatio = ratios;
    int streamCount = streamsRatio.size();
    QVector<QSize> itemSizes;
    QVector<QPoint> itemPos;
    double scale_size = 1.0 / 3.0;
    double scale_pos_up = 1.0 / 10.0;
    double scale_pos_down = 1.0 / 2.0;
    if (pipMode)
    {
        //index 0 always full screen.
        itemSizes.push_back(fullSize);
        QSize smallSize(fullSize.width()* scale_size, fullSize.height()* scale_size);
        for (int i = 1; i < streamCount; ++i)
        {
            QSize eachSize = smallSize;
            //eachSize.scale(eachSize.width() * 1.0, eachSize.height() * arrRatios[i], Qt::KeepAspectRatioByExpanding);
            itemSizes.push_back(eachSize);
        }
        itemPos = { QPoint(0, 0), QPoint(fullSize.width()*scale_pos_up, fullSize.height()*scale_pos_up),
            QPoint(fullSize.width()*scale_pos_up, fullSize.height()*scale_pos_down),
            QPoint(fullSize.width()*scale_pos_down, fullSize.height()*scale_pos_down) };
    }
    else
    {
        QVector<QPoint> itemPos_4 = { QPoint(0, 0), QPoint(0, fullSize.height() / 2) ,QPoint(fullSize.width() / 2, 0), QPoint(fullSize.width() / 2, fullSize.height() / 2) };
        if (streamCount > 1)
        {
            QSize oneSize = fullSize / 2;
            for (int i = 0; i < streamCount; i++)
            {
                itemSizes.push_back(oneSize);
                if (streamCount == 2)
                    itemPos.push_back(QPoint(fullSize.width() / 4, i * fullSize.height() / 2));
                else
                    itemPos.push_back(itemPos_4[i]);
            }
        }
        else
        {
            itemSizes.push_back(fullSize);
            itemPos.push_back(QPoint(0, 0));
        }
    }

    int nIndex = 0;
    for (auto iter : streamsRatio)
    {
        itemSizePos.insert(make_pair(iter.first, QRect(itemPos[nIndex], itemSizes[nIndex])));
        nIndex++;
    }

    return itemSizePos;
}

DWORD Utility::getProcessIdByName(const QString& processName )
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each proces.
    for (i = 0; i < cProcesses; i++)
    {
        DWORD processID = aProcesses[i];
        if (processID != 0)
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

            // Get a handle to the process.

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, processID);

            // Get the process name.

            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName,
                        sizeof(szProcessName) / sizeof(TCHAR));
                }
            }
            
            // Release the handle to the process.
            CloseHandle(hProcess);

            QString curProcessName = QString::fromUtf16((ushort*)szProcessName);
            if (curProcessName  == processName)
                return processID;
        }
    }

    return -1;
}

bool Utility::isProcessRunning()
{
    QString captureProcessName = "Capture.exe";
    wchar_t* processName = (wchar_t*)captureProcessName.utf16();

    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
    {
        while (Process32Next(snapshot, &entry))
        {
            if (!wcsicmp(entry.szExeFile, processName))
            {
                exists = true;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    return exists;
}

QSharedPointer<QSettings> Utility::applicationSettings()
{
    QDir appDataFolder = QProcessEnvironment::systemEnvironment().value("LocalAppData");
    QString strPathName = appDataFolder.path() + "/HP/WorkTools/Show/config.ini";
    qInfo() << "The customized path file is in " << strPathName;

    return QSharedPointer<QSettings>::create(strPathName, QSettings::IniFormat);
}
