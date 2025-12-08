#ifndef _HYSTERESIS_FILTER_H_
#define _HYSTERESIS_FILTER_H_

#include "FilterBase.h"

namespace Filter
{

template <typename T> class Hysteresis : public Base<T>
{
public:
    Hysteresis(T hysVal)
    {
        this->Reset();
        this->hysValue = hysVal;
    }

    virtual T GetNext(T value)
    {
        if (FILTER_ABS(value - this->lastValue) > this->hysValue)
        {
            this->lastValue = value;
        }
        return this->lastValue;
    }

private:
    T hysValue;
};

}

#endif
