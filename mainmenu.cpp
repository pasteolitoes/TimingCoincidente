#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "calibration.h"
#include "taskchoice.h"
#include "taskwindow.h"
#include "IMUConnection.h"

MainMenu::MainMenu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenu),
    _timer(Q_NULLPTR)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    IMUConnection con;
    QString port = con.checkPort();
    if(port != nullptr)
    {
        ui->portIMU->setEnabled(true);
        ui->portIMU->addItem(port);

        ui->portIMU->setCurrentIndex(0);
        portName = port;
        imuCon_ = true;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QSettings settings(path + "/TimingCoincidente.ini", QSettings::IniFormat);
    settings.beginGroup("Valores Aquisição");

    int rep = settings.value("Repetições Aquis").toInt();
    int vel = settings.value("Velocidade Aquis").toInt();

    settings.beginGroup("Valores Transferência");

    int rep2 = settings.value("Repetições Transf").toInt();
    int vel2 = settings.value("Velocidade Transf").toInt();

    if(rep!=0 && vel!=0 && rep2!=0 && vel2!=0)
    {
        ui->trainingRepBox->setValue(rep);
        ui->trainingVelBox->setValue(vel);

        ui->acquiRepBox->setValue(rep2);
        ui->acquiVelBox->setValue(vel2);
    }
}

MainMenu::~MainMenu()
{
    delete ui;
}

//Inicia o programa
void
MainMenu::
on_startButton_clicked()
{
    if(imuCon_ == false)
    {
        QMessageBox msgBox("Sensor Inercial (IMU) não detectado",
                                 "O sensor inercial não foi detectado ou não está conectado."
                                 "Deseja continuar com os testes sem os parâmetros inerciais?",
                                 QMessageBox::Warning,
                                 QMessageBox::Ok,
                                 QMessageBox::Cancel,
                                 QMessageBox::NoButton);
        if (msgBox.exec() == QMessageBox::RejectRole)
            return;
    }

    //Abre diálogo para seleção do tipo de tarefa
    TaskChoice t_choice;

    t_choice.setModal(true);

    if (t_choice.exec() == QDialog::Accepted)
    {
        task = t_choice.get();

        //Caso Acquisição selecionado
        if (task == 1){
            velocity = ui->trainingVelBox->text().toInt();
            repetitions = ui->trainingRepBox->text().toInt();
        }
        //Caso Tranferência selecionada
        else if (task == 2){
            velocity = ui->acquiVelBox->text().toInt();
            repetitions = ui->acquiRepBox->text().toInt();
        }
    } else
    {
        return;
    }

    if (ui->flexButton->isChecked())
    {
        cam = FLEX13;
    } else if (ui->webcamButton->isChecked())
    {
        cam = WEBCAM;
    }

    TaskWindow maintask(this);
    maintask.setInfo(cam, repetitions, velocity, task, portName);
    maintask.exec();

}

void MainMenu::on_calibrationButton_clicked()
{
//    this->hide();

}

//Acessa a tela de configurações da tarefa
void
MainMenu::
on_configButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


//Volta os valores das variáveis de treino e aquisição para os padrões
void
MainMenu::
on_defaultButton_clicked()
{
    ui->acquiRepBox->setValue(7);
    ui->acquiVelBox->setValue(200);
    ui->trainingRepBox->setValue(21);
    ui->trainingVelBox->setValue(500);
}

//Aplica os valores alterados da configuração para as variáveis de treino aquisição
//e volta para o menu inicial
void
MainMenu::
on_applyButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QSettings settings(path + "/TimingCoincidente.ini", QSettings::IniFormat);
    settings.beginGroup("Valores Aquisição");

    int rep = ui->trainingRepBox->value();
    settings.setValue("Repetições Aquis", rep);
    int vel = ui->trainingVelBox->value();
    settings.setValue("Velocidade Aquis", vel);

    settings.beginGroup("Valores Transferência");

    int rep2 = ui->acquiRepBox->value();
    settings.setValue("Repetições Transf", rep2);
    int vel2 = ui->acquiVelBox->value();
    settings.setValue("Velocidade Transf", vel2);
}

//Fecha o programa
void
MainMenu::
on_exitButton_clicked()
{
    this->close();
}

void
MainMenu::
on_checkButton_clicked()
{
    IMUConnection con;

    QString port = con.checkPort();
    if(port != nullptr)
    {
        ui->portIMU->setEnabled(true);
        ui->portIMU->addItem(port);
        ui->portIMU->setCurrentIndex(0);
        portName = port;
        imuCon_ = true;
    }
    else
    {
        ui->portIMU->clear();
        ui->portIMU->setEnabled(false);
        portName = nullptr;
    }
}
