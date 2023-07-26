#ifndef _H264_PARSER_H_
#define _H264_PARSER_H_

#include "h264_stream.h"

class H264Parser
{
public:
	h264_stream_t* m_h264;

public:
	UINT process(uint8 *pdata, size_t size);

public:
	H264Parser();
	virtual ~H264Parser();
};

#endif
