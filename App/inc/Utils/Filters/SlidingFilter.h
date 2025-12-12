#ifndef __SLIDING_FILTER_H
#define __SLIDING_FILTER_H

#include "FilterBase.h"

namespace Filter
{

template <typename T> class Sliding : public Base<T>
{
public:
    Sliding(T sldVal)
    {
        this->Reset();
        this->slideValue = sldVal;
    }

    virtual T GetNext(T value)
    {
        if (this->CheckFirst())
        {
            this->lastValue = value;
        }
        else
        {
            if (FILTER_ABS(value - this->lastValue) < this->slideValue)
            {
                this->lastValue = value;
            }
            else
            {
                if (this->lastValue < value)
                {
                    this->lastValue += this->slideValue;
                }
                else if (this->lastValue > value)
                {
                    this->lastValue -= this->slideValue;
                }
            }
        }
        return this->lastValue;
    }

private:
    T slideValue;
};

}

#endif
