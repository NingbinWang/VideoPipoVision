#ifndef __LOWPASS_FILTER_H
#define __LOWPASS_FILTER_H

#include "FilterBase.h"

namespace Filter
{

template <typename T> class Lowpass : public Base<T>
{
public:
    Lowpass(float dt, float cutoff)
    {
        this->Reset();

        this->dT = dt;
        if (cutoff > 0.001f)
        {
            float RC = 1 / (2 * 3.141592653f * cutoff);
            this->rc = dt / (RC + dt);
        }
        else
        {
            this->rc = 1;
        }
    }

    virtual T GetNext(T value)
    {
        if (this->CheckFirst())
        {
            return this->lastValue = value;
        }
        else
        {
            this->lastValue = (this->lastValue + (value - this->lastValue) * this->rc);
            return this->lastValue;
        }
    }

private:
    float dT;
    float rc;
};

}

#endif
