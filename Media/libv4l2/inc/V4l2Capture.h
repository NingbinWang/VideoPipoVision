/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** V4l2Capture.h
** 
** V4L2 Capture wrapper 
**
** -------------------------------------------------------------------------*/


#pragma once 

#include "V4l2Access.h"

// ---------------------------------
// V4L2 Capture
// ---------------------------------
class V4l2Capture : public V4l2Access
{		
	protected:	
		explicit V4l2Capture(V4l2Device* device);
	
	public:
		static V4l2Capture* create(const V4L2DeviceParameters & param);
		static V4l2Capture* create(const V4L2DeviceParameters & param,v4l2_buf_type type);
		virtual ~V4l2Capture();
	
		size_t read(char* buffer, size_t bufferSize);
#ifdef MEDIARKMPP
		void * readtobuf(int* index);
		bool   readputbuf(int index);
#endif
		bool   isReadable(timeval* tv);	
		bool   capturepoll();
};


