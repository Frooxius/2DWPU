#pragma once
#include <QObject>
#include <QThread>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>
#include <cmath>
#include "2DWPU.h"

using namespace WPU2D::Core;

class WPU2Dthread : public QThread
{
    Q_OBJECT

	WPU2Dcore **wpu;
    volatile float *freq;
    volatile bool *maxhz;
    volatile bool stop;
    QTime time;
    QMutex StopMutex;

public:
    WPU2Dthread(WPU2Dcore **wpu, float *frequency, bool *unlimited);
    void run();
    void StartIfNotRunning() { if(!isRunning()) start(QThread::NormalPriority); sleep(0); }
    void Stop();

signals:
    void UpdateUI();
	void Halt();
};
