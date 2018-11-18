#ifndef CONNECTIONWORKER_H
#define CONNECTIONWORKER_H

#include <QObject>
#include "IMUConnection.h"

class ConnectionWorker : public QObject
{
    Q_OBJECT
public:
    explicit
    ConnectionWorker(QString portName, QString workspace);

signals:
    void
    finished();

public slots:
    void
    endWorker();

    void
    startConnection();

    void
    getTime();

private:
    IMUConnection* connection;

    QString workspace_;
    QString portName_;

};

#endif // CONNECTIONWORKER_H
