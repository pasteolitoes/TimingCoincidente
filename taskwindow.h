#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#define WEBCAM 0
#define FLEX13 1

#include <QDialog>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QVector>
#include <QThread>
#include <QMediaPlayer>
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv2\videoio.hpp>
#include <cameralibrary.h>
#include <Windows.h>
#include "chronometer.h"

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:

    TaskWindow(QWidget *parent);

    /* ------Variáveis básicas da tarefa------ */
    int taskType;
    int taskRep;
    int taskVel;
    int cam;

    /* ------Variáveis do alvo------*/
    double area;
    bool targetFound = false;
    const int MIN_OBJECT_AREA = 200;
    const int MAX_OBJECT_AREA = 30000;

    /* ------Variáveis da câmera e da imagem------ */
    int camWidth;
    int camHeight;
    const int WIDTH = 640;
    const int HEIGHT = 480;
    const int BACKBUFFER_BITSPERPIXEL = 8;

    cv::VideoCapture capture;
    CameraLibrary::Camera *camFlex;





    /*------Variáveis de calibração e processamento------*/

    int block_t = 323;
    int gaussC_t = 75;
    int mult_t = 8;
    int thr_t = 0;


public:

    void
    setInfo(int camera, int rep, int vel, int type, QString portName);

    void
    startCamera(int camera);

    void
    firstState();

signals:
    void
    finishedTask();

    void
    startRecord();

    void
    finishRecord();

    void
    saveTime();

private slots:
    void
    on_startTaskButton_clicked();

    void
    on_exitButton_clicked();

    void
    nextState();

    void
    updateTime();

    void
    restartState();

    void
    onFinished();



private:
    void
    findTarget(cv::Mat &inFrame, cv::Mat &outFrame);


private:
    Ui::TaskWindow *ui;

    QTimer clockTimer;
    QDir workspace;

    QVector<QTime> times;
    QVector<qint64> times2;
    QVector<qint64> temporizador;
    QVector<QTime> timeStart;
    QVector<QTime> timeEnd;
    QVector<int> score;
    QVector<int> difference;

    Chronometer c;
    QString portName_;

    int state = -1;
    int doneCount = 0;
    int flag = 0;
    int nBlocks = 10;
    QTime t0;
    QTime tHand;
    QTime tAlvo;

    bool firstFlag = false;
    bool breakLoop = false;
    bool finalFlag = false;
    bool FIM = false;
    int resultado = 0;
};

#endif // TASKWINDOW_H
