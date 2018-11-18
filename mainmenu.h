#ifndef MAINMENU_H
#define MAINMENU_H

#define WEBCAM 0
#define FLEX13 1

#include <QMainWindow>
#include <QDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStandardPaths>
#include <sstream>
#include <string>
#include <iostream>
#include <thread>


#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv2\videoio.hpp>
#include <cameralibrary.h>
#include <QDebug>
#include <windows.h>

namespace Ui {
class MainMenu;
}

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = 0);

    ~MainMenu();

    //Flag para testes
    int flag = 0;

    /*------Variáveis básicas e do alvo-----*/

    int task;
    int repetitions;
    int velocity;

    double area;

    int score = 0;
    bool targetFound = false;
    const int MIN_OBJECT_AREA = 200;
    const int MAX_OBJECT_AREA = 3000;

    /*-----Variáveis da câmera e da imagem-----*/

    int cam;
    int camWidth;
    int camHeight;
    const int WIDTH = 640;
    const int HEIGHT = 480;
    const int BACKBUFFER_BITSPERPIXEL = 8;

    /*------Variáveis de calibração e processamento------*/

    int block_t = 323;
    int gaussC_t = 75;
    int mult_t = 8;
    int thr_t = 0;

    QString portName;

public:

    void
    startCamera();

    void
    findTarget(cv::Mat &cameraFeed, cv::Mat &binaryImage);

private slots:
    void
    on_configButton_clicked();

    void
    on_startButton_clicked();

    void
    on_defaultButton_clicked();

    void
    on_applyButton_clicked();

    void
    on_exitButton_clicked();

    void
    on_calibrationButton_clicked();

    void
    on_checkButton_clicked();

private:
    Ui::MainMenu *ui;

    QTimer* _timer;

    bool imuCon_ = false;

};

#endif // MAINMENU_H
