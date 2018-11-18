#include "IMUConnection.h"
#include <QDebug>

IMUConnection::IMUConnection()
{
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readPortData()));
}

QString
IMUConnection::
checkPort()
{
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    if (serialPortInfos.count() >= 1)
    {
        for(const QSerialPortInfo &serialPortInfo : serialPortInfos)
        {
            if(serialPortInfo.serialNumber() == "FTU53VC0A")
            {
                return serialPortInfo.portName();
            }
        }
    }
    return nullptr;
}

void
IMUConnection::
openSerialPort(QString port)
{
    serial->setPortName(port);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite))
    {
        QMessageBox msg("ERRO AO ABRIR A IMU", "Não foi possível abrir a IMU.",
                        QMessageBox::Critical, QMessageBox::NoButton, QMessageBox::NoButton, QMessageBox::Close);
        imuError = true;

        qDebug() << "Deu erro na IMU!!";

    } else {
        setVNBaudRate(921600);

        QByteArray b = "VNWRG,06,7";

        writeRegister(b);

        QByteArray c = "VNWRG,07,200";

        writeRegister(c);

        serial->waitForReadyRead(500);

        QTime t;
        tStart = t.currentTime().toString("hh:mm:ss.zzz");

        tempor.start();

        qDebug() << "Abriu (?) a IMU!";
    }

    flag = 1;
}

void
IMUConnection::
closeSerialPort(QString workspace)
{
    QString tEnd = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz");

    QByteArray b = "VNWRG,6,0";

    writeRegister(b);

    QString filePath = workspace + "/TC_IMU_" + QDateTime::currentDateTime().toString("dd_MM-hh_mm") + ".txt";

    if (imuError == false)
    {
        QFile file(filePath);

        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&file);

        out << tStart << endl;

        for(int j = 0; j < resultado.size(); j++ )
        {
            out << resultado.at(j);
        }

        out << endl;
        out << tEnd;
//        out << QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz");

        file.close();

        resultado.clear();

        QString filePath2 = workspace + "/TEMPOS_IMU" + QDateTime::currentDateTime().toString("dd_MM-hh_mm") + ".txt";
        QFile file2(filePath2);
        file2.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out2(&file2);

        out2 << tStart << endl;
        for(int k = 0; k < tempos.size();k++)
        {
            out2 << tempos.at(k) << endl;
        }
        out2 << tEnd;
        file2.close();
        tempos.clear();

    }

    flag = 0;

    if (serial->isOpen())
        serial->close();
}

void
IMUConnection::
writePortData(const QByteArray &data)
{
    serial->write(data);
    serial->waitForBytesWritten();
}

void
IMUConnection::
readPortData()
{
    QByteArray data = serial->readAll();
    resultado.append(data);

    if(data.isNull())
    {
        qDebug() << "Não está entrando nada";
    }
}

void IMUConnection::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        serial->close();
    }
}

void IMUConnection::markTime()
{
    tempos.append(tempor.elapsed());
}

void
IMUConnection::
setVNBaudRate(qint32 value)
{
    writeRegister("VNWRG,6,0");
    QThread::msleep(250);
    serial->clear();

    QByteArray str = "VNWRG,5," + QByteArray::number(value);

    QThread::msleep(250);
    writeRegister(str);

    QThread::msleep(250);
    serial->setBaudRate(value);

}

void
IMUConnection
::writeRegister(QByteArray string)
{
    int i;
    quint8 cksum = 0;

    for(i = 0; i < string.length(); i++)
    {
        cksum ^= string.at(i);
    }

    QString s = "$" + string + "*" + QString::number(cksum,16).toUpper() + "\n";

    writePortData(s.toLocal8Bit());

}




