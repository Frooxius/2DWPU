#include "2DWPUthread.h"

WPU2Dthread::WPU2Dthread(WPU2Dcore **wpu, float *frequency, bool *unlimited)
{
    this->wpu = wpu;
    time = QTime();
    freq = frequency;
    maxhz = unlimited;
    StopMutex.lock();
}

void WPU2Dthread::Stop()
{
    stop = true;
    if(isRunning())
        StopMutex.lock();
}

void WPU2Dthread::run()
{
    time.start();

    int updatems,   // how many milliseconds need to pass for a screen update
        updatecycles, // how many cycles to do in each step
        cyclesdelta; // how many cycles were processed so far

    for(;;)
    {
        if(*maxhz)
        {
            updatems = 33;
        }
        else
        {
            updatems = std::max(1000.0/(*freq), 33.0);
            updatecycles = (*freq)/(1000/updatems);
            cyclesdelta = 0;
        }
        time.restart();

		int updates = 1;
		if(*freq >= 1000.0f || *maxhz)
			updates = 1000;
        do
        {
            if(stop)
            {
                stop = false;
                StopMutex.unlock();
                return;
            }
            if(!(*wpu)->Cycle(updates))
			{
				// if it requests halt, then stop it
				emit Halt();
				break;
			}
            cyclesdelta+=updates;
        } while( (cyclesdelta < updatecycles || *maxhz) && time.elapsed() < updatems);

		if(!(*maxhz))
		{
		     emit UpdateUI();
		     msleep(std::min(std::max(updatems-time.elapsed(), 0), 1000));
		}
    }
}
