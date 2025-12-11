#include "PingPongBuffer.h"
#include <string.h>

/**
  * @brief  Ping-pong buffer initialization
  * @param  ppbuf: Pointer to the ping-pong buffer structure
  * @param  buf0:  Pointer to the first buffer
  * @param  buf1:  Pointer to the second buffer
  * @retval None
  */
void PingPongBuffer_Init(PingPongBuffer_t* ppbuf, void* buf0, void* buf1)
{
    memset(ppbuf, 0, sizeof(PingPongBuffer_t));
    ppbuf->buffer[0] = buf0;
    ppbuf->buffer[1] = buf1;
}

/**
  * @brief  Get a readable buffer
  * @param  ppbuf:     Pointer to the ping-pong buffer structure
  * @param  pReadBuf:  Pointer to the pointer to the buffer to be read
  * @retval Returns true if there is a buffer to be read
  */
bool PingPongBuffer_GetReadBuf(PingPongBuffer_t* ppbuf, void** pReadBuf)
{
    if(ppbuf->readAvaliable[0])
    {
        ppbuf->readIndex = 0;
    }
    else if(ppbuf->readAvaliable[1])
    {
        ppbuf->readIndex = 1;
    }
    else
    {
        return false;
    }
    *pReadBuf = ppbuf->buffer[ppbuf->readIndex];
    return true;
}

/**
  * @brief  Notify buffer read completion
  * @param  ppbuf: Pointer to the ping-pong buffer structure
  * @retval None
  */
void PingPongBuffer_SetReadDone(PingPongBuffer_t* ppbuf)
{
    ppbuf->readAvaliable[ppbuf->readIndex] = false;
}

/**
  * @brief  Get writable buffer
  * @param  ppbuf:      Pointer to the ping-pong buffer structure
  * @param  pWriteBuf:  Pointer to the pointer to the buffer to be wriye
  * @retval None
  */
void PingPongBuffer_GetWriteBuf(PingPongBuffer_t* ppbuf, void** pWriteBuf)
{
    if(ppbuf->writeIndex == ppbuf->readIndex)
    {
        ppbuf->writeIndex = !ppbuf->readIndex;
    }
    *pWriteBuf = ppbuf->buffer[ppbuf->writeIndex];
}

/**
  * @brief  Notify buffer write completion
  * @param  ppbuf: Pointer to the ping-pong buffer structure
  * @retval None
  */
void PingPongBuffer_SetWriteDone(PingPongBuffer_t* ppbuf)
{
    ppbuf->readAvaliable[ppbuf->writeIndex] = true;
    ppbuf->writeIndex = !ppbuf->writeIndex;
}
