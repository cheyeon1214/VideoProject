#include "pch.h"
#include "H264DEC.h"

H264DEC::H264DEC()
{
	c_dec = NULL;
	picture_dec = NULL;

	av_init_packet(&avpkt);
}
H264DEC::~H264DEC()
{
	Exit();

	av_packet_unref(&avpkt);
}

bool H264DEC::Init()
{
	AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if(!codec) return false;

	c_dec = avcodec_alloc_context3(codec);
	if(avcodec_open2(c_dec, codec, NULL) < 0){
		return false;
	}

	picture_dec = av_frame_alloc();

	return true;
}
void H264DEC::Exit()
{
	if(c_dec != NULL)
	{
		avcodec_close(c_dec);
		av_free(c_dec);
		c_dec = NULL;
	}
	if(picture_dec != NULL)
	{
		av_frame_free(&picture_dec);
		picture_dec = NULL;
	}
}
bool H264DEC::Decode(uint8* pInput, int nInsize, VID_IMGSRC * pVid_img)
{
	avpkt.data = pInput;
	avpkt.size = nInsize;

	int got_picture = 0;
	if(avcodec_decode_video2(c_dec, picture_dec, &got_picture, &avpkt) < 0) return false;

	if(got_picture > 0)
	{
		pVid_img->f_col = FOURCC_YV12;
		pVid_img->hsize = c_dec->width;
		pVid_img->vsize = c_dec->height;
		pVid_img->y_src = picture_dec->data[0];
		pVid_img->u_src = picture_dec->data[1];
		pVid_img->v_src = picture_dec->data[2];
		pVid_img->y_stride = picture_dec->linesize[0];
		switch(c_dec->pix_fmt)
		{
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUVJ420P:
			pVid_img->uv_stride = picture_dec->linesize[1];
			break;
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUVJ422P:
			pVid_img->uv_stride = 2 * picture_dec->linesize[1];
			break;
		}
	}
	return true;
}
