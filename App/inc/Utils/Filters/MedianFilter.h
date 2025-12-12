#ifndef __MEDIAN_FILTER_H
#define __MEDIAN_FILTER_H

#include "FilterBase.h"
#include <algorithm>

namespace Filter
{

template <typename T, size_t bufferSize> class Median : public Base<T>
{
public:
    Median()
    {
        this->Reset();
        this->dataIndex = 0;
    }

    bool FillBuffer(T value)
    {
        if (this->dataIndex < bufferSize)
        {
            this->buffer[this->dataIndex] = value;
            this->dataIndex++;
            return false;
        }
        return true;
    }

    virtual T GetNext(T value)
    {
        if (this->isFirst)
        {
            this->isFirst = !FillBuffer(value);
            this->lastValue = value;
        }
        else
        {
            if (FillBuffer(value))
            {
                std::sort(this->buffer, this->buffer + bufferSize);
                this->lastValue = this->buffer[bufferSize / 2];
                this->dataIndex = 0;
            }
        }

        return this->lastValue;
    }

protected:
    T buffer[bufferSize];
    size_t dataIndex;
};

}

#endif
