#pragma once

#if LIBAVCODEC_VERSION_MAJOR > 56

class H264ENC
{
private:
	AVCodecContext* c_enc;
	AVFrame* picture_yuv;
	uint8* m_ptr;

public:
	bool Init(int hsize, int vsize, int fps, int quality);
	void Exit();
	bool Encode(VID_IMGSRC * pVid_img, uint8* pOutput, int & nOutsize, BOOL & bRecvI);

public:
	H264ENC();
	virtual ~H264ENC();
};

#endif
