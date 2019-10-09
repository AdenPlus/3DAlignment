include($$PWD/Registration/fpm.pri)

QT += core widgets gui multimedia multimediawidgets concurrent svg opengl
CONFIG   += c++11 force_debug_info

DEFINES += GLEW_STATIC

INCLUDEPATH += \
    $$PWD/Registration/base \
    $$PWD/Registration/components \
    $$PWD/Registration/core \
    $$PWD/Registration/model \
    $$PWD/Registration/util \
    $$PWD/Registration/window\
    $$PWD/Registration/render\
    "C:\Program Files\PCL 1.8.1\include\pcl-1.8" \
    "C:\Program Files\PCL 1.8.1\3rdParty\Boost\include\boost-1_64" \
    "C:\Program Files\PCL 1.8.1\3rdParty\Eigen\eigen3" \
    $$PWD/ThirdParty/include/GLFramework \
    "C:\Program Files\PCL 1.8.1\3rdParty\FLANN\include"

INCLUDEPATH += $$PWD \
    $$PWD/ThirdParty/include/GLFramework/glew/include \

TARGET = 3DRegistration
TEMPLATE = app
DESTDIR = $$PWD/build/$$FORTIS_SPEC/Registration

SOURCES += \
    $$PWD/Registration/base/*.cpp \
    $$PWD/Registration/components/*.cpp \
    $$PWD/Registration/core/*.cpp \
    $$PWD/Registration/model/*.cpp \
    $$PWD/Registration/window/*.cpp \
    $$PWD/Registration/util/*.cpp \
    $$PWD/Registration/render/*.cpp \
    $$PWD/Registration/main.cpp \

HEADERS += \
    $$PWD/Registration/base/*.h \
    $$PWD/Registration/components/*.h \
    $$PWD/Registration/core/*.h \
    $$PWD/Registration/model/*.h \
    $$PWD/Registration/window/*.h \
    $$PWD/Registration/util/*.h \
    $$PWD/Registration/render/*.h \

FORMS += \
    $$PWD/Registration/window/forms/*.ui \

RESOURCES += \
    $$PWD/Registration/Registration.qrc

OTHER_FILES += \
    $$PWD/package.json \
    $$PWD/CHANGELOG.md

QMAKE_LIBDIR += "C:/Program Files/PCL 1.8.1/lib" \
                                "C:/Program Files/PCL 1.8.1/3rdParty/Boost/lib" \
                                $$PWD/ThirdParty/lib/GLFramework/glew/lib

LIBS += -lopengl32 -lglu32 -lglew32s
win32:LIBS += -lUser32 -lShell32 -lWtsapi32 -lwevtapi -lgdi32 -lD3D11 -ldxgi -lgdiplus -lDbghelp -lStrmiids -lShcore

win32:CONFIG(release, debug|release):  LIBS += -lpcl_common_release -lpcl_features_release \
    -lpcl_filters_release -lpcl_io_ply_release -lpcl_io_release -lpcl_kdtree_release -lpcl_keypoints_release \
    -lpcl_ml_release -lpcl_octree_release -lpcl_outofcore_release -lpcl_people_release -lpcl_recognition_release \
    -lpcl_registration_release -lpcl_sample_consensus_release -lpcl_search_release -lpcl_segmentation_release \
    -lpcl_stereo_release -lpcl_surface_release -lpcl_tracking_release -lpcl_visualization_release \
    -llibboost_thread-vc140-mt-1_64

win32:CONFIG(debug, debug|release):  LIBS += -lpcl_common_debug -lpcl_features_debug \
    -lpcl_filters_debug -lpcl_io_ply_debug -lpcl_io_debug -lpcl_kdtree_debug -lpcl_keypoints_debug \
    -lpcl_ml_debug -lpcl_octree_debug -lpcl_outofcore_debug -lpcl_people_debug -lpcl_recognition_debug \
    -lpcl_registration_debug -lpcl_sample_consensus_debug -lpcl_search_debug -lpcl_segmentation_debug \
    -lpcl_stereo_debug -lpcl_surface_debug -lpcl_tracking_debug -lpcl_visualization_debug \
    -llibboost_thread-vc140-mt-gd-1_64

RC_ICONS += $$PWD/Registration/icons/Registration.ico

win32:CONFIG(release, debug|release): QMAKE_POST_LINK += $(COPY_FILE) \
                                                        $$system_quote($$shell_path($$PWD/bin/win32-msvc2015-x64-r)) \
                                                        $$system_quote($$shell_path($$DESTDIR)) $$escape_expand(\\n\\t)

#this code is required due to QTBUG-48416
#the bug states that lrelease doesn't run on subdirs
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

win32:QMAKE_LFLAGS += /INCREMENTAL /debug:fastlink

WINDEPLOYQT_ARGS += -multimediawidgets -websockets --no-translations
deployDependencies()
fillTargetMetaData($$PWD/package.json)
extractMetaData($$PWD/package.json)
