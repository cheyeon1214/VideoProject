#pragma once

class H264DEC
{
private:
	AVCodecContext* c_dec;
	AVFrame* picture_dec;

	AVPacket avpkt;

public:
	bool Init();
	void Exit();
	bool Decode(uint8* pInput, int nInsize, VID_IMGSRC * pVid_img);

public:
	H264DEC();
	virtual ~H264DEC();
};
