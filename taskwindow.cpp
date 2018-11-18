#include "taskwindow.h"
#include "ui_taskwindow.h"
#include "ConnectionWorker.h"

using namespace CameraLibrary;
using namespace std;

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));

    connect(this, SIGNAL(finishedTask()), this, SLOT(onFinished()));

    timeStart.clear();
    timeEnd.clear();
    times2.clear();
    times.clear();
}

//Seta as informações necessárias para a realização da tarefa que
//foram configuradas a partir do menu principal
void
TaskWindow::
setInfo(int camera, int rep, int vel, int type, QString portName)
{
    cam = camera;
    taskRep = rep;
    taskType = type;
    taskVel = vel;
    portName_ = portName;

    if (taskType == 1)
    {
        ui->typeLabel->setText("Treino");
    }
    else if (taskType == 2)
    {
        ui->typeLabel->setText("Aquisição");
    }

    ui->repLabel->setText("0/"+QString::number(taskRep));

    QDir currentDir = QApplication::applicationDirPath();
    QString startingPath = currentDir.absolutePath();

    workspace.setPath(QFileDialog ::getExistingDirectory(nullptr,
                                                   "Selecione a pasta onde serão salvos os arquivos do teste",
                                                   startingPath));
    qDebug() << workspace.path();

    return;
}


void TaskWindow::on_startTaskButton_clicked()
{
    QElapsedTimer important;
//    important.start();

//    qDebug() << "Relógio é do tipo: " << important.clockType();

//    if(important.isMonotonic())
//        qDebug() << "Relógio é Monotônico.";

    //Open Thread and worker to save the IMU data on a file
    QThread *workerThread = new QThread;
    ConnectionWorker* worker = new ConnectionWorker(portName_, workspace.path());

    worker->moveToThread(workerThread);

    connect(this, SIGNAL(startRecord()), worker, SLOT(startConnection()));
    connect(this, SIGNAL(finishRecord()), worker, SLOT(endWorker()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    connect(this, SIGNAL(saveTime()), worker, SLOT(getTime()));
    workerThread->start(QThread::HighPriority);

    ui->startTaskButton->hide();
    ui->exitButton->hide();
    std::string windowName = "Tarefa de TC";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    if (cam == WEBCAM)
    {
        cv::Mat matFrame;

        //Inicializa a webcam
        capture.open(0);
        capture.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

        emit startRecord();

        while(true)
        {
            capture >> matFrame;

            cv::imshow(windowName, matFrame);

            if(GetAsyncKeyState(0x44))
            {
                cv::destroyAllWindows();
                emit finishRecord();
                capture.release();

                break;
            }

            cv::waitKey(10);
        }

    }
    //Inicia a câmera FLEX13 e começa a gravação de imagem
    else if (cam == FLEX13)
    {
        CameraManager::X().WaitForInitialization();
        camFlex = CameraManager::X().GetCamera();

        if(!camFlex){
            QMessageBox msg(QMessageBox::Warning,
                            "ERRO CÂMERA", "Não foi encontrada a câmera Flex13.");
            this->accept();
        }

        camFlex->SetVideoType(Core::GrayscaleMode);
        camFlex->SetExposure(50);
        camFlex->SetThreshold(200);
        camFlex->SetIntensity(15);

        camFlex->Start();
        camWidth = camFlex->Width();
        camHeight = camFlex->Height();

        cv::Mat matFrame(cv::Size(camWidth, camHeight), CV_8UC1);
        cv::Mat finalFrame;
        matFrame.copyTo(finalFrame);

        Frame *frame = camFlex->GetFrame();

        emit startRecord();

        QMediaPlayer *player = new QMediaPlayer;
        QMediaPlayer *player2 = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/sounds/216090__richerlandtv__bad-beep-incorrect.mp3"));
        player2->setMedia(QUrl("qrc:/sounds/243701__ertfelda__correct.mp3"));

        while(true)
        {
//            temporizador.append(important.nsecsElapsed());
            //termina a captura de imagens
            if(breakLoop == true) break;

            //captura das imagens
            frame = camFlex->GetFrame();

            if(frame)
            {
                frame->Rasterize(camWidth, camHeight, matFrame.step, BACKBUFFER_BITSPERPIXEL, matFrame.data);
                frame->Release();
            }

            //Quando a tecla "i" é pressionada, inicia a tarefa, mudando o estado para o inicial
            if(GetAsyncKeyState(0x49))
            {
                //Início da tarefa, quando "i" é pressionado pela primeira vez
                if (firstFlag == false)
                {
                    ui->timeLabel->setText(QTime(0, 0, 0, 0).toString());
                    c.restart();
                    firstFlag = true;
                    clockTimer.start(50);
                    times.append(QTime::currentTime());
                }

                if (state == -1)
                {
                    emit saveTime();
                    t0 = c.getTime();
                    timeStart.append(t0);
                    times2.append(c.getElapsedTime());
                    firstState();
                    doneCount++;
                    ui->repLabel->setText(QString::number(doneCount)+
                                          "/"+QString::number(taskRep));
                    tAlvo = t0.addMSecs((nBlocks-1)*taskVel);

                }
            }

//            Se não for a primeira vez e não for o estado que não foi iniciado
            if (firstFlag == true && state != -1)
            {
                //Se o voluntário não apertou o botão antes do tempo, errou
                if(state == (nBlocks-1) && -(c.getTime().msecsTo(t0)) >= taskVel*(state) + 2000 && FIM == false){
//                    qDebug() << c.getTime().msecsTo(t0);
                    FIM = true;
                    score.append(0);
//                    QSound::play(":/sounds/216090__richerlandtv__bad-beep-incorrect.mp3");
                    player->play();
                    timeEnd.append(c.getTime());
                    times2.append(c.getElapsedTime());
                    restartState();

                    if(doneCount >= taskRep)
                    {
                        QTimer::singleShot(2000,worker,SIGNAL(finishRecord));
    //                    QTimer::singleShot()
    //                    emit finishRecord();
                        times.append(QTime::currentTime());
                        times2.append(c.getElapsedTime());

                        cv::destroyAllWindows();

                        camFlex->Stop();
                        camFlex->Release();
                        ui->exitButton->show();
                        clockTimer.stop();
                        c.restart();

                        emit finishRecord();

                        emit finishedTask();
                    }

                }
//                Se o estado for menor que o número de blocos e o tempo for maior do que o daquele bloco, muda de estado
                else if(state < (nBlocks-1) && (-(c.getTime().msecsTo(t0)) >= taskVel*(state+1))){
                    nextState();
                }
            }

            //Caso o resultado já tenha sido adquirido, tocar o som após 500ms
            if(firstFlag == true && state == -1 && resultado != 0 && -(c.getTime().msecsTo(tHand)) >= 1500)
            {       
                if(resultado == 1)
                    player2->play();
//                    QSound::play(":/sounds/243701__ertfelda__correct.mp3");

                else if (resultado == 2)
                    player->play();
//                    QSound::play(":/sounds/216090__richerlandtv__bad-beep-incorrect.mp3");

                resultado = 0;

                if(doneCount >= taskRep)
                {
                    QTimer::singleShot(2000,worker,SIGNAL(finishRecord));
//                    QTimer::singleShot()
//                    emit finishRecord();
                    times.append(QTime::currentTime());
                    times2.append(c.getElapsedTime());

                    cv::destroyAllWindows();

                    camFlex->Stop();
                    camFlex->Release();
                    ui->exitButton->show();
                    clockTimer.stop();
                    c.restart();

                    emit finishRecord();

                    emit finishedTask();
                }
            }

            threshold(matFrame, matFrame, 200, 255, cv::THRESH_BINARY);

            findTarget(matFrame, finalFrame);

            //Marcação do tempo
//            temporizador.append(important.nsecsElapsed());

            imshow(windowName, finalFrame);

//            Caso a tecla "e" seja pressionada, destrói as janelas e para o programa
            if(GetAsyncKeyState(0x45)){

                cv::destroyAllWindows();
                emit finishRecord();
                camFlex->Stop();
                camFlex->Release();
                ui->exitButton->show();
                clockTimer.stop();
                c.restart();

                break;
            }

//            temporizador.append(important.nsecsElapsed());

            cv::waitKey(2);

        }
//        camFlex->Stop();
//        camFlex->Release();
//        ui->exitButton->show();
//        clockTimer.stop();
//        c.restart();

    }

//    temporizador.append(important.nsecsElapsed());


//    QString filePath = workspace.path() + "/tempo.txt";
//    QFile file(filePath);
//    file.open(QIODevice::WriteOnly | QIODevice::Text);

//    QTextStream out(&file);

//    for(int o = 0; o < temporizador.size(); o++)
//    {
//        out << temporizador.at(o) << endl;
//    }

    this->accept();
}

void
TaskWindow::on_exitButton_clicked()
{
    this->close();
}

void
TaskWindow::firstState()
{
    state = 0;

    finalFlag = false;
}

void
TaskWindow::nextState()
{
    state++;
    qDebug() << "Next state: " << state;
}

void
TaskWindow::
restartState()
{
    state = -1;
    flag = 0;

    qDebug() << doneCount;

    FIM = false;
}


void
TaskWindow::updateTime()
{
    ui->timeLabel->setText(c.getTime().toString("hh:mm:ss.zzz"));
}

void
TaskWindow::findTarget(cv::Mat &matFrame, cv::Mat &finalFrame)
{
    vector<vector<cv::Point> > contornos;
    vector<cv::Point> approx;

    int alvo = 0;
    int quadrado = 0;
    int formas = 0;

    cv::Scalar color(66, 238, 244);
    cv::Scalar color_g(38, 249, 14);
    cv::Scalar color_c(194, 196, 198);
    cv::Scalar color_w(255,255,255);

    cv::findContours(matFrame, contornos, cv::RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


    for (int i = 0; i < contornos.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(contornos[i]), approx, 0.02 * mult_t * cv::arcLength(cv::Mat(contornos[i]), true), true);

        area = cv::contourArea(contornos[i]);

        if (area > MIN_OBJECT_AREA)
        {
            if (approx.size() == 4)
            {
                quadrado = i;
                alvo ++;
                targetFound = true;
                formas++;
            }
        }


        if(FIM == false && alvo == 0 && flag == 0 && state >= 0)
        {
            FIM = true;
            targetFound = 0;
            tHand = c.getTime();
//            tAlvo = t0.addMSecs((nBlocks-1)*taskVel);
            int dif = tAlvo.msecsTo(tHand);
            difference.append(dif);
            qDebug() << "tHand: " << tHand;
            qDebug() << "tAlvo: " << tAlvo;
            qDebug() << "Diferença " << dif;


            //Se acertou o alvo na janela de +- 100ms
            if (dif > -100 && dif < 100)
            {
                flag++;
                score.append(1);
                timeEnd.append(c.getTime());
                times2.append(c.getElapsedTime());
                restartState();
                resultado = 1;
            }
            else
            {
                score.append(0);
                flag++;
                timeEnd.append(c.getTime());
                times2.append(c.getElapsedTime());
                restartState();
                resultado = 2;
            }

        }


    }
    cv::cvtColor(matFrame, finalFrame, cv::COLOR_GRAY2BGR);
    if (formas > 0){
        if(state >= 0){
            for(int i = 0; i < (nBlocks-1); i++){
                if(state < (nBlocks-1)){
                    if (i == 0){
                        cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8);
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2);
                    }
                    if(((nBlocks-2)-state) == i){
                        cv::drawContours(finalFrame, contornos, quadrado, color, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                    }else {
                        cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                    }
                } else {
                    if(i == 0){
                        cv::drawContours(finalFrame, contornos, quadrado, color_g, CV_FILLED, cv::LINE_8);
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2);
                    }
                    cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                    cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                }
            }
        }
    }
}

void
TaskWindow::onFinished()
{
    breakLoop = true;

//    emit finishRecord();

    QString filePath = workspace.path() + "/TesteTC_" + QTime::currentTime().toString("dd_MM-hh_mm") + ".txt";
    QFile file(filePath);

    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream out(&file);

    out << "Tarefa de Timing Coincidente realizada em: " << QDateTime::currentDateTime().toString() << endl;

    if(taskType == 1){
        out << "Velocidade da tarefa de Treino: " <<  QString::number(taskVel) << endl;
    }
    else if (taskType == 2)
    {
        out << "Velocidade da tarefa de Aquisição: " << QString::number(taskVel) << endl;
    }

    out << "START TIME: " << times.at(0).toString("hh:mm:ss.zzz") << endl;

    out << "Start;Start(ms);End;End(ms);Score;Delay" << endl;

    for(int i = 0; i < score.size(); i++)
    {
        out << QString::number(i+1) << "\t" << timeStart.at(i).toString("hh:mm:ss.zzz") <<"\t"<< QString::number(times2.at(2*i)) <<"\t";
        out << timeEnd.at(i).toString("hh:mm:ss.zzz") <<"\t"<< QString::number(times2.at((2*i)+1)) <<"\t";
        out << QString::number(score.at(i)) <<"\t"<< QString::number(difference.at(i)) << endl;

        qDebug() << difference.size();
    }

    out << "END TIME: " << times.at(1).toString("hh:mm:ss.zzz");

    timeStart.clear();
    timeEnd.clear();
    times2.clear();
    times.clear();

    file.close();
}
