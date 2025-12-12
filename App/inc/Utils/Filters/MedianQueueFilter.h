#ifndef __MEDIAN_QUEUE_FILTER_H
#define __MEDIAN_QUEUE_FILTER_H

#include "MedianFilter.h"

namespace Filter
{

template <typename T, size_t bufferSize> class MedianQueue : public Median<T, bufferSize>
{
public:
    MedianQueue() : Median<T, bufferSize>()
    {
    }

    virtual T GetNext(T value)
    {
        if (this->isFirst)
        {
            this->isFirst = !this->FillBuffer(value);
            this->lastValue = value;
        }
        else
        {
            this->dataIndex %= bufferSize;
            this->buffer[this->dataIndex] = value;
            this->dataIndex++;

            for (size_t i = 0; i < bufferSize; i++)
            {
                this->bufferSort[i] = this->buffer[i];
            }
            std::sort(this->bufferSort, this->bufferSort + bufferSize);
            this->lastValue = this->bufferSort[bufferSize / 2];
        }

        return this->lastValue;
    }

protected:
    T bufferSort[bufferSize];
};

}

#endif
