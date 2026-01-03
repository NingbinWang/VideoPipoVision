/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** V4l2MmapDevice.cpp
** 
** V4L2 source using mmap API
**
** -------------------------------------------------------------------------*/

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iostream>

// libv4l2
#include <linux/videodev2.h>

// project
#include "Logger.h"
#include "V4l2MmapDevice.h"

V4l2MmapDevice::V4l2MmapDevice(const V4L2DeviceParameters & params, v4l2_buf_type deviceType) : V4l2Device(params, deviceType), n_buffers(0) 
{
	memset(&m_buffer, 0, sizeof(m_buffer));
}

bool V4l2MmapDevice::init(unsigned int mandatoryCapabilities)
{
	bool ret = V4l2Device::init(mandatoryCapabilities);
	if (ret)
	{
		ret = this->start();
	}
	return ret;
}

V4l2MmapDevice::~V4l2MmapDevice()
{
	this->stop();
}


bool V4l2MmapDevice::start() 
{
	 std::cout<< "Device " << m_params.m_devName <<std::endl;

	bool success = true;
	struct v4l2_requestbuffers req;
	memset (&req, 0, sizeof(req));
	req.count               = V4L2MMAP_NBBUFFER;
	req.type                = m_deviceType;
	req.memory              = V4L2_MEMORY_MMAP;
    std::cout<< "req.count: " << req.count << "req.type:" << req.type << "req.memory:" << req.memory <<std::endl;
	if (-1 == ioctl(m_fd, VIDIOC_REQBUFS, &req)) 
	{
		if (EINVAL == errno) 
		{
			std::cout << "Device " << m_params.m_devName << " does not support memory mapping" <<std::endl;
			success = false;
		} 
		else 
		{
			std::cout << "Device " << m_params.m_devName << " VIDIOC_REQBUFS error"<<std::endl ;
			perror("VIDIOC_REQBUFS");
			success = false;
		}
	}
	else
	{
		std::cout<< "Device " << m_params.m_devName << " nb buffer:" << req.count <<std::endl;
		// allocate buffers
		memset(&m_buffer,0, sizeof(m_buffer));
		for (n_buffers = 0; n_buffers < req.count; ++n_buffers) 
		{
			struct v4l2_buffer buf;
			memset (&buf, 0, sizeof(buf));
			buf.type        = m_deviceType;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = n_buffers;
			struct v4l2_plane planes[FMT_NUM_PLANES];
			if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
            	buf.m.planes = planes;
           		buf.length = FMT_NUM_PLANES;
            }

			if (-1 == ioctl(m_fd, VIDIOC_QUERYBUF, &buf))
			{
				std::cout<< "Device " << m_params.m_devName << " VIDIOC_QUERYBUF error" <<std::endl;
				perror("VIDIOC_QUERYBUF");
				success = false;
			}
			else
			{
				std::cout<< "Device " << m_params.m_devName << " buffer idx:" << n_buffers << " size:" << buf.length << " offset:" << buf.m.offset <<std::endl;
				if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf.type) {
					m_buffer[n_buffers].length = buf.m.planes[0].length;
					m_buffer[n_buffers].start = mmap (   NULL /* start anywhere */, 
											 buf.m.planes[0].length,
											PROT_READ | PROT_WRITE /* required */, 
											MAP_SHARED /* recommended */, 
											m_fd, 
											buf.m.planes[0].m.mem_offset);
				}else{
					m_buffer[n_buffers].length = buf.length;
					if (!m_buffer[n_buffers].length) {
						m_buffer[n_buffers].length = buf.bytesused;
					}
					m_buffer[n_buffers].start = mmap (   NULL /* start anywhere */, 
											m_buffer[n_buffers].length, 
											PROT_READ | PROT_WRITE /* required */, 
											MAP_SHARED /* recommended */, 
											m_fd, 
											buf.m.offset);
				}

				if (MAP_FAILED == m_buffer[n_buffers].start)
				{
					perror("mmap");
					success = false;
				}
#ifdef MEDIARKMPP
        		struct v4l2_exportbuffer expbuf = (struct v4l2_exportbuffer) {0} ;
        		// xcam_mem_clear (expbuf);
       			expbuf.type = m_deviceType;
      			expbuf.index = n_buffers;
        		expbuf.flags = O_CLOEXEC;
				
        		if( ioctl(m_fd, VIDIOC_EXPBUF, &expbuf) < 0) {
            			perror("VIDIOC_EXPBUF");
        		} else {
            		std::cout<<"get dma buf(" << n_buffers << ")-fd: " <<expbuf.fd << std::endl;
           			MppBufferInfo info;
            		memset(&info, 0, sizeof(MppBufferInfo));
            		info.type = MPP_BUFFER_TYPE_EXT_DMA;
            		info.fd =  expbuf.fd;
            		info.size = m_buffer[n_buffers].length & 0x07ffffff;
            		info.index = (m_buffer[n_buffers].length & 0xf8000000) >> 27;
            		mpp_buffer_import(&m_buffer[n_buffers].start, &info);
					m_buffer[n_buffers].export_fd = expbuf.fd;
        		}
#endif
			}
		}

		// queue buffers
		for (unsigned int i = 0; i < n_buffers; ++i) 
		{
			struct v4l2_buffer buf;
			struct v4l2_plane planes[FMT_NUM_PLANES];
			memset (&buf, 0, sizeof(buf));
			buf.type        = m_deviceType;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;
			std::cout<< " VIDIOC_QBUF buf idx:" << i << std::endl;
			if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
            	buf.m.planes = planes;
           		buf.length = FMT_NUM_PLANES;
            }
			
			if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf))
			{
				perror("VIDIOC_QBUF");
				success = false;
				return success; 
			}
		}
		std::cout<< " VIDIOC_STREAMON start" << std::endl;
		// start stream
		int type = m_deviceType;
		if (-1 == ioctl(m_fd, VIDIOC_STREAMON, &type))
		{
			perror("VIDIOC_STREAMON");
			success = false;
		}
		//skip some frames at start
		/*
		for(unsigned int i = 0; i < n_buffers; ++i){
		    struct v4l2_buffer buf = {0};
			buf.type   = m_deviceType;
        	buf.memory = V4L2_MEMORY_MMAP;
			struct v4l2_plane planes[FMT_NUM_PLANES];
    		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
        			buf.m.planes = planes;
        			buf.length = FMT_NUM_PLANES;
    		}
			if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &buf))
			{
				perror("VIDIOC_DQBUF");
				success = false;
				return success; 
			}
			 if (buf.index > n_buffers) {
        		perror("buffer index out of bounds\n");
       			success = false;
				return success; 
             }
			if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType)
           		buf.bytesused = buf.m.planes[0].bytesused;
			int idx = buf.index;
			if (idx >= 0)
			{
				buf = (struct v4l2_buffer) {0};
			    buf.type   = m_deviceType;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index  = idx;
			    struct v4l2_plane planes[FMT_NUM_PLANES];
   			    if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == type) {
        			buf.m.planes = planes;
        			buf.length = FMT_NUM_PLANES;
    		    }
			    if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf))
			    {
				  perror("VIDIOC_QBUF");
				   success = true;
			   }
                success = true;
			}
		}
		*/
	}	
	return success; 
}

bool V4l2MmapDevice::stop() 
{
	std::cout << "Device " << m_params.m_devName <<std::endl;

	bool success = true;
	
	int type = m_deviceType;
	if (-1 == ioctl(m_fd, VIDIOC_STREAMOFF, &type))
	{
		perror("VIDIOC_STREAMOFF");      
		success = false;
	}

	for (unsigned int i = 0; i < n_buffers; ++i)
	{
		if (-1 == munmap (m_buffer[i].start, m_buffer[i].length))
		{
			perror("munmap");
			success = false;
		}
	}
	
	// free buffers
	struct v4l2_requestbuffers req;
	memset (&req, 0, sizeof(req));
	req.count               = 0;
	req.type                = m_deviceType;
	req.memory              = V4L2_MEMORY_MMAP;
	if (-1 == ioctl(m_fd, VIDIOC_REQBUFS, &req)) 
	{
		perror("VIDIOC_REQBUFS");
		success = false;
	}
	
	n_buffers = 0;
	return success; 
}

size_t V4l2MmapDevice::readInternal(char* buffer, size_t bufferSize)
{
	size_t size = 0;
	LOG_INFO("buffer=%p buffersize=%ld\n",buffer,bufferSize);
	if(buffer == NULL|| bufferSize == 0)
		return -1;
	if (n_buffers > 0)
	{
		struct v4l2_buffer buf;	
		memset (&buf, 0, sizeof(buf));
		buf.type = m_deviceType;
		buf.memory = V4L2_MEMORY_MMAP;
		struct v4l2_plane planes[FMT_NUM_PLANES];
    	if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
        			buf.m.planes = planes;
        			buf.length = FMT_NUM_PLANES;
    	}

		if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &buf)) 
		{
			if (errno == EAGAIN) {
				size = 0;
			} else {
				perror("VIDIOC_DQBUF");
				size = -1;
			}
		}
		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType)
           		buf.bytesused = buf.m.planes[0].bytesused;

		if (buf.index < n_buffers)
		{
			size = buf.bytesused;
			if (size > bufferSize)
			{
				size = bufferSize;
				std::cout << "Device " << m_params.m_devName << " buffer truncated available:" << bufferSize << " needed:" << buf.bytesused <<std::endl ;
			}
#ifdef MEDIARKMPP
			MppBuffer mppbuffer = m_buffer[buf.index].start;
			void *ptr = mpp_buffer_get_ptr(mppbuffer);
			size_t length = mpp_buffer_get_size(mppbuffer);
			if (size > length)
			{
				size = length;
				std::cout << "Device " << m_params.m_devName << " buffer truncated available:" << bufferSize << " needed:" << length <<std::endl ;
			}
			LOG_INFO("ptr = %p length= %ld buffer=%p buffersize=%ld\n",ptr,length,buffer,bufferSize);
			memcpy(buffer, ptr, size);
#else
			memcpy(buffer, m_buffer[buf.index].start, size);
#endif
			if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf))
			{
				perror("VIDIOC_QBUF");
				size = -1;
			}
		}else{
			LOG_ERROR("buffer index out of bounds\n");
			size = -1;
		}
	}
	return size;
}

#ifdef MEDIARKMPP
MppBuffer V4l2MmapDevice::readtorkbuf(int* index)
{
	  MppBuffer buffer = NULL;
	//int ret = -1;
	if (n_buffers > 0)
	{
		struct v4l2_buffer buf;	
		memset (&buf, 0, sizeof(buf));
		buf.type = m_deviceType;
		buf.memory = V4L2_MEMORY_MMAP;
		struct v4l2_plane planes[FMT_NUM_PLANES];
    	if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
        			buf.m.planes = planes;
        			buf.length = FMT_NUM_PLANES;
    	}

		if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &buf)) 
		{
			if (errno == EAGAIN) {
				return nullptr;
			} else {
				perror("VIDIOC_DQBUF");
				return nullptr;
			}
		}
		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType)
           		buf.bytesused = buf.m.planes[0].bytesused;

		if (buf.index < n_buffers)
		{
			buffer = m_buffer[buf.index].start;
			mpp_buffer_sync_end(buffer);
			*index = buf.index;
		}else{
			LOG_ERROR("buffer index out of bounds\n");
			return nullptr;
		}
	}
	return buffer;
}

bool V4l2MmapDevice::readputrkbuf(int index)
{
    struct v4l2_buffer buf;

    if (index < 0)
        return false;

    buf = (struct v4l2_buffer) {0};
    buf.type   = m_deviceType;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = index;

    struct v4l2_plane planes[FMT_NUM_PLANES];
    if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == m_deviceType) {
        buf.m.planes = planes;
        buf.length = FMT_NUM_PLANES;
    }

    // Tell kernel it's ok to overwrite this frame
    if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf)) {
        perror("VIDIOC_QBUF\n");
        return false;
    }

    return true;
}
#endif


size_t V4l2MmapDevice::writeInternal(char* buffer, size_t bufferSize)
{
	size_t size = 0;
	if (n_buffers > 0)
	{
		struct v4l2_buffer buf;	
		memset (&buf, 0, sizeof(buf));
		buf.type = m_deviceType;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &buf)) 
		{
			perror("VIDIOC_DQBUF");
			size = -1;
		}
		else if (buf.index < n_buffers)
		{
			size = bufferSize;
			if (size > buf.length)
			{
				std::cout << "Device " << m_params.m_devName << " buffer truncated available:" << buf.length << " needed:" << size <<std::endl;
				size = buf.length;
			}
			memcpy(m_buffer[buf.index].start, buffer, size);
			buf.bytesused = size;

			if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf))
			{
				perror("VIDIOC_QBUF");
				size = -1;
			}
		}
	}
	return size;
}

bool V4l2MmapDevice::startPartialWrite()
{
	if (n_buffers <= 0)
		return false;
	if (m_partialWriteInProgress)
		return false;
	memset(&m_partialWriteBuf, 0, sizeof(m_partialWriteBuf));
	m_partialWriteBuf.type = m_deviceType;
	m_partialWriteBuf.memory = V4L2_MEMORY_MMAP;
	if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &m_partialWriteBuf))
	{
		perror("VIDIOC_DQBUF");
		return false;
	}
	m_partialWriteBuf.bytesused = 0;
	m_partialWriteInProgress = true;
	return true;
}

size_t V4l2MmapDevice::writePartialInternal(char* buffer, size_t bufferSize)
{
	size_t new_size = 0;
	size_t size = 0;
	if ((n_buffers > 0) && m_partialWriteInProgress)
	{
		if (m_partialWriteBuf.index < n_buffers)
		{
			new_size = m_partialWriteBuf.bytesused + bufferSize;
			if (new_size > m_partialWriteBuf.length)
			{
				std::cout << "Device " << m_params.m_devName << " buffer truncated available:" << m_partialWriteBuf.length << " needed:" << new_size <<std::endl;
				new_size = m_partialWriteBuf.length;
			}
			size = new_size - m_partialWriteBuf.bytesused;
			memcpy(&((char *)m_buffer[m_partialWriteBuf.index].start)[m_partialWriteBuf.bytesused], buffer, size);

			m_partialWriteBuf.bytesused += size;
		}
	}
	return size;
}

bool V4l2MmapDevice::endPartialWrite()
{
	if (!m_partialWriteInProgress)
		return false;
	if (n_buffers <= 0)
	{
		m_partialWriteInProgress = false; // abort partial write
		return true;
	}
	if (-1 == ioctl(m_fd, VIDIOC_QBUF, &m_partialWriteBuf))
	{
		perror("VIDIOC_QBUF");
		m_partialWriteInProgress = false; // abort partial write
		return true;
	}
	m_partialWriteInProgress = false;
	return true;
}
