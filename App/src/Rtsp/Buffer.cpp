#include "Buffer.h"
#include <unistd.h>
#include <sys/uio.h>
#include "SysFile.h"
const int Buffer::initialSize = 1024;
const char* Buffer::kCRLF = "\r\n";

int Buffer::read(int fd)
{
    // saved an ioctl()/FIONREAD call to tell how much to read
    char extrabuf[65536];
    struct iovec vec[2];
    const int writable = writableBytes();
    vec[0].iov_base = begin()+mWriteIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most.
    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    const int n = readv(fd, vec, iovcnt);
    if (n < 0)
    {
        return -1;
    }
    else if (n <= writable)
    {
        mWriteIndex += n;
    }
    else
    {
        mWriteIndex = mBufferSize;
        append(extrabuf, n - writable);
    }

    return n;
}

int Buffer::write(int fd)
{
    return SysFile_write(fd, peek(), readableBytes());
}