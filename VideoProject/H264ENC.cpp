#include "pch.h"
#include "H264ENC.h"

#if LIBAVCODEC_VERSION_MAJOR > 56

H264ENC::H264ENC()
{
	c_enc = NULL;
	picture_yuv = NULL;

	m_ptr = NULL;
}
H264ENC::~H264ENC()
{
	Exit();
}

bool H264ENC::Init(int hsize, int vsize, int fps, int quality)
{
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if(!codec) return false;

	c_enc = avcodec_alloc_context3(codec);

	c_enc->profile = FF_PROFILE_H264_MAIN;//FF_PROFILE_H264_BASELINE, FF_PROFILE_H264_MAIN, FF_PROFILE_H264_HIGH
	c_enc->pix_fmt = AV_PIX_FMT_YUV420P;
	c_enc->width = hsize;
	c_enc->height = vsize;
	c_enc->bit_rate = quality;
	c_enc->time_base.num = 1;
    c_enc->time_base.den = fps;
	c_enc->gop_size = fps;
	c_enc->max_b_frames = 0;

	c_enc->qcompress = 1.0;
	c_enc->qmin = 3;
	c_enc->qmax = 31;

	av_opt_set(c_enc->priv_data, "profile", "main", 0); // baseline, main, high
	av_opt_set(c_enc->priv_data, "tune", "zerolatency", 0);
	//av_opt_set(c_enc->priv_data, "preset", "fast", 0);

	if(avcodec_open2(c_enc, codec, NULL) < 0){
		return false;
	}

	picture_yuv = av_frame_alloc();

	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, hsize, vsize, 1);
	m_ptr = (uint8*)av_malloc(numBytes);
	if(av_image_fill_arrays(picture_yuv->data, picture_yuv->linesize, m_ptr, AV_PIX_FMT_YUV420P, hsize, vsize, 1) < 0)
		return false;
	picture_yuv->width = hsize;
	picture_yuv->height = vsize;
	picture_yuv->format = AV_PIX_FMT_YUV420P;

	return true;
}
void H264ENC::Exit()
{
	if(c_enc != NULL)
	{
		avcodec_close(c_enc);
		av_free(c_enc);
		c_enc = NULL;
	}
	if(picture_yuv != NULL)
	{
		av_frame_free(&picture_yuv);
		picture_yuv = NULL;
	}
	if(m_ptr != NULL)
	{
		av_free(m_ptr);
		m_ptr = NULL;
	}
}
bool H264ENC::Encode(VID_IMGSRC * pVid_img, uint8* pOutput, int & nOutsize, BOOL & bRecvI)
{
	if(pVid_img->f_col != FOURCC_YV12)
		return false;
	if(pVid_img->hsize != picture_yuv->width)
		return false;
	if(pVid_img->vsize != picture_yuv->height)
		return false;

	uint8 * y_dst = picture_yuv->data[0];
	uint8 * u_dst = picture_yuv->data[1];
	uint8 * v_dst = picture_yuv->data[2];
	int y_dst_stride = picture_yuv->linesize[0];
	int uv_dst_stride = picture_yuv->linesize[1];

	uint8 * y_src = pVid_img->y_src;
	uint8 * u_src = pVid_img->u_src;
	uint8 * v_src = pVid_img->v_src;
	int y_src_stride = pVid_img->y_stride;
	int uv_src_stride = pVid_img->uv_stride;
			
	int width = pVid_img->hsize;
	int height = pVid_img->vsize;
	int width2 = width / 2;
	int height2 = height / 2;

	for(int y = height; y; y--)
	{
		memcpy(y_dst, y_src, width);
		y_src += y_src_stride;
		y_dst += y_dst_stride;
	}
	for(int y = height2; y; y--)
	{
		memcpy(u_dst, u_src, width2);
		u_src += uv_src_stride;
		u_dst += uv_dst_stride;
	}
	for(int y = height2; y; y--)
	{
		memcpy(v_dst, v_src, width2);
		v_src += uv_src_stride;
		v_dst += uv_dst_stride;
	}

	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.data = NULL;
	avpkt.size = 0;

	int got_picture = 0;
	if(avcodec_encode_video2(c_enc, &avpkt, picture_yuv, &got_picture) < 0) return false;

	if(got_picture > 0)
	{
		if(nOutsize < avpkt.size) nOutsize = 0;
		else
		{
			memcpy(pOutput, avpkt.data, avpkt.size);
			nOutsize = avpkt.size;

			bRecvI = FALSE;
			if(avpkt.size > 4)
			{
				uint8 start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
				uint8 start_cod2[3] = { 0x00, 0x00, 0x01 };

				uint8* buf = pOutput;

				bool b_start_code = false;
				if(!memcmp(buf, start_code, 4))
				{
					buf += 4;
					b_start_code = true;
				}
				else if(!memcmp(buf, start_cod2, 3))
				{
					buf += 3;
					b_start_code = true;
				}
				if(b_start_code)
				{
					uint8 nal_type = *buf & 0x1f;
					if(nal_type == 0x07) bRecvI = TRUE;
				}
			}
		}
	}
	else nOutsize = 0;

	av_packet_unref(&avpkt);

	return true;
}

#endif
