#include "ConnectionWorker.h"

ConnectionWorker::ConnectionWorker(QString portName, QString workspace)
{
    connection = new IMUConnection();

    workspace_ = workspace;
    portName_ = portName;
}

void
ConnectionWorker::
startConnection()
{
    connection->openSerialPort(portName_);
}


void
ConnectionWorker::
endWorker()
{
    connection->closeSerialPort(workspace_);

    emit finished();
}

void
ConnectionWorker::
getTime()
{
    connection->markTime();
}
