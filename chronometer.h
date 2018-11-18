#ifndef CHRONOMETER_H
#define CHRONOMETER_H

#include <QTime>
#include <QElapsedTimer>


class Chronometer
{
public:
    Chronometer();

    void
    restart();

    QTime getTime();

    qint64
    getElapsedTime();

public:
    QElapsedTimer t;
};

#endif // CHRONOMETER_H
