#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T13:37:09
#
#-------------------------------------------------

QT       += core gui serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoincidenteTiming
TEMPLATE = app
CONFIG += c++11 console

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainmenu.cpp \
    calibration.cpp \
    calibration.cpp \
    taskchoice.cpp \
    taskwindow.cpp \
    chronometer.cpp \
    IMUConnection.cpp \
    ConnectionWorker.cpp

HEADERS += \
        mainmenu.h \
    calibration.h \
    calibration.h \
    taskchoice.h \
    taskwindow.h \
    chronometer.h \
    IMUConnection.h \
    ConnectionWorker.h

FORMS += \
        mainmenu.ui \
    calibration.ui \
    taskchoice.ui \
    taskwindow.ui

INCLUDEPATH += "D:\\opencv-build\\install\\include" \
               "D:\\opencv-build\\install\\include\\opencv" \
               "D:\\opencv-build\\install\\include\\opencv2"

INCLUDEPATH += "D:\\Program Files\\OptiTrack\\Camera SDK\\include"

LIBS += -L"D:\\Program Files\\OptiTrack\\Camera SDK\\lib" \
-lCameraLibrary2015x64S

LIBS += -L"D:\\opencv-build\\install\\x64\\vc14\\lib" \
-lopencv_core330 \
-lopencv_calib3d330 \
-lopencv_dnn330 \
-lopencv_features2d330 \
-lopencv_flann330 \
-lopencv_highgui330 \
-lopencv_imgcodecs330 \
-lopencv_imgproc330 \
-lopencv_ml330 \
-lopencv_objdetect330 \
-lopencv_photo330 \
-lopencv_shape330 \
-lopencv_stitching330 \
-lopencv_superres330 \
-lopencv_video330 \
-lopencv_videoio330 \
-lopencv_videostab330 \
-lopencv_core330d \
-lopencv_calib3d330d \
-lopencv_dnn330d \
-lopencv_features2d330d \
-lopencv_flann330d \
-lopencv_highgui330d \
-lopencv_imgcodecs330d \
-lopencv_imgproc330d \
-lopencv_ml330d \
-lopencv_objdetect330d \
-lopencv_photo330d \
-lopencv_shape330d \
-lopencv_stitching330d \
-lopencv_superres330d \
-lopencv_video330d \
-lopencv_videoio330d \
-lopencv_videostab330d

LIBS += -lUser32

RESOURCES += \
    sounds.qrc
