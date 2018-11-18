#ifndef IMUCONNECTION_H
#define IMUCONNECTION_H

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QElapsedTimer>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class IMUConnection : public QSerialPort
{
    Q_OBJECT

public:
    IMUConnection();

public:

    QString
    checkPort();

    void
    openSerialPort(QString port);

    void
    closeSerialPort(QString workspace);

    void
    writePortData(const QByteArray &data);

    void
    writeRegister(QByteArray string);

    void
    setVNBaudRate(qint32 value);

    void
    handleError(QSerialPort::SerialPortError error);

    void
    markTime();

public:
    QSerialPort *serial;


public slots:
    void
    readPortData();


private:
    int flag = 0;

    bool imuError = false;

    QByteArray temp;

    QVector<QString> resultado;

    QString tStart;

    QVector<qint64> tempos;

    QElapsedTimer tempor;

};

#endif // IMUCONNECTION_H
