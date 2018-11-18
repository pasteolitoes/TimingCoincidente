#include "chronometer.h"

Chronometer::Chronometer()
{

}

void
Chronometer::restart()
{
    t.restart();
}

QTime
Chronometer::getTime()
{
    return QTime(0,0,0,0).addMSecs(t.elapsed());
}

qint64 Chronometer::getElapsedTime()
{
    return t.elapsed();
}
