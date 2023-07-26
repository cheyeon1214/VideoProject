#include "stdafx.h"
#include "H264Parser.h"

H264Parser::H264Parser()
{
	m_h264 = h264_new();
}
H264Parser::~H264Parser()
{
	h264_free(m_h264);
	m_h264 = NULL;
}

UINT H264Parser::process(uint8 *pdata, size_t size)
{
	uint8* p = pdata;
	size_t sz = size;

	int nal_start, nal_end;

	while(find_nal_unit(p, sz, &nal_start, &nal_end) > 0)
	{
		p += nal_start;
		read_nal_unit(m_h264, p, nal_end - nal_start);

		switch(m_h264->nal->nal_unit_type)
		{
		case NAL_UNIT_TYPE_SPS:
		case NAL_UNIT_TYPE_PPS: break;
		default: return (UINT)m_h264->nal->nal_unit_type; break;
		}

		p += (nal_end - nal_start);
		sz -= nal_end;
	}
	return NAL_UNIT_TYPE_UNSPECIFIED;
}
