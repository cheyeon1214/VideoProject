#ifndef _DX_DEFS_H_
#define _DX_DEFS_H_

#ifdef _WIN64
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/d3d9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/d3dx9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/dsound.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/dxguid.lib")
#else
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/d3d9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/d3dx9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/dsound.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/dxguid.lib")
#endif
#pragma comment(lib, "vfw32.lib")

#include "d3d9.h"
#include "d3dx9.h"
#include "dsound.h"

#define MAX_IMG			4
#define SCR_IMG_BLK		0
#define SCR_IMG_LOGO	1
#define SCR_IMG_RECON	2
#define SCR_IMG_DISCON	3
#define SCR_CAM_CONNECT	4

static WCHAR *szImgName[MAX_IMG] =
{
	L"blk.bmp",
	L"logo.bmp",
	L"recon.bmp",
	L"discon.bmp"
};

inline bool _cmn_col_copy(VID_IMGSRC * pVid_dst, VID_IMGSRC * pVid_src)
{
	if(pVid_dst->f_col != pVid_src->f_col)
		return false;
	switch(pVid_dst->f_col)
	{
	case FOURCC_YV12:
		{
			uint8 * y_dst = pVid_dst->y_src;
			uint8 * u_dst = pVid_dst->u_src;
			uint8 * v_dst = pVid_dst->v_src;
			int y_dst_stride = pVid_dst->y_stride;
			int uv_dst_stride = pVid_dst->uv_stride;

			uint8 * y_src = pVid_src->y_src;
			uint8 * u_src = pVid_src->u_src;
			uint8 * v_src = pVid_src->v_src;
			int y_src_stride = pVid_src->y_stride;
			int uv_src_stride = pVid_src->uv_stride;
			
			int width = pVid_src->hsize;
			int height = pVid_src->vsize;
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
			return true;
		}
		break;
	case FOURCC_BGRA:
		{
			uint8 * x_dst = pVid_dst->x_ptr;
			int x_dst_stride = pVid_dst->x_stride;

			uint8 * x_src = pVid_src->x_ptr;
			int x_src_stride = pVid_src->x_stride;
			
			int width = pVid_src->hsize;
			int height = pVid_src->vsize;
			int width4 = width * 4;

			for(int y = height; y; y--)
			{
				memcpy(x_dst, x_src, width4);
				x_src += x_src_stride;
				x_dst += x_dst_stride;
			}
			return true;
		}
		break;
	}
	return false;
}

/*****************************************************************************
 *
 *  OffSurfCtrl : Offscreen
 *
 ****************************************************************************/

struct OffSurfCtrl
{
	uint32	f_col;
	int		hsize;
	int		vsize;
	IDirect3DSurface9* lpOffSurf;

	OffSurfCtrl()
	{
		this->f_col = 0;
		this->hsize = 0;
		this->vsize = 0;
		this->lpOffSurf = NULL;
	}
	~OffSurfCtrl()
	{
		Clear();
	}
	inline void Clear()
	{
		SAFE_RELEASE(this->lpOffSurf);
		this->f_col = 0;
		this->hsize = 0;
		this->vsize = 0;
	}
	inline BOOL IsSurf(VID_IMGSRC* pVid_img)
	{
		if(!this->lpOffSurf) return FALSE;
		if(this->f_col != pVid_img->f_col) return FALSE;
		if(this->hsize != pVid_img->hsize) return FALSE;
		if(this->vsize != pVid_img->vsize) return FALSE;
		return TRUE;
	}
	inline BOOL Create(IDirect3DDevice9* lpD3DD, VID_IMGSRC* pVid_img)
	{
		SAFE_RELEASE(this->lpOffSurf);

		this->f_col = pVid_img->f_col;
		this->hsize = pVid_img->hsize;
		this->vsize = pVid_img->vsize;

		HRESULT hr = lpD3DD->CreateOffscreenPlainSurface(
			this->hsize, this->vsize,
			(D3DFORMAT)(this->f_col == FOURCC_BGRA ? D3DFMT_X8R8G8B8 : this->f_col),
			D3DPOOL_DEFAULT,
			&this->lpOffSurf,
			0);
		if(SUCCEEDED(hr)) return TRUE;

		Clear();

		return FALSE;
	}
	inline BOOL Draw(IDirect3DDevice9* lpD3DD, VID_IMGSRC* pVid_img)
	{
		if(!IsSurf(pVid_img))
		{
			if(!Create(lpD3DD, pVid_img)) return FALSE;
		}

		D3DLOCKED_RECT locked;
		switch(this->f_col)
		{
		case FOURCC_YV12:
			{
				HRESULT hr = this->lpOffSurf->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
				if(FAILED(hr)) return FALSE;

				BOOL bRet = FALSE;
				switch(pVid_img->f_col)
				{
				case FOURCC_YV12:
					{
						VID_IMGSRC stVid_dst;
						stVid_dst.f_col = this->f_col;
						stVid_dst.y_src = (uint8*)locked.pBits;
						stVid_dst.v_src = (stVid_dst.y_src + locked.Pitch*this->vsize);
						stVid_dst.u_src = (stVid_dst.v_src + locked.Pitch*this->vsize/4);
						stVid_dst.y_stride = locked.Pitch;
						stVid_dst.uv_stride = locked.Pitch/2;

						_cmn_col_copy(&stVid_dst, pVid_img);
					}
					bRet = TRUE;
					break;
				}

				this->lpOffSurf->UnlockRect();

				return bRet;
			}
			break;
		case FOURCC_BGRA:
			{
				HRESULT hr = this->lpOffSurf->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
				if(FAILED(hr)) return FALSE;

				BOOL bRet = FALSE;
				switch(pVid_img->f_col)
				{
				case FOURCC_BGRA:
					{
						VID_IMGSRC stVid_dst;
						stVid_dst.f_col = this->f_col;
						stVid_dst.x_ptr = (uint8*)locked.pBits;
						stVid_dst.x_stride = locked.Pitch;

						_cmn_col_copy(&stVid_dst, pVid_img);
					}
					bRet = TRUE;
					break;
				}

				this->lpOffSurf->UnlockRect();

				return bRet;
			}
			break;
		}
		return FALSE;
	}
};

/*****************************************************************************
 *
 *  DX_SCREEN
 *
 ****************************************************************************/

struct DX_SCREEN
{
	void* DxSurf;
	int H_size;
	int V_size;
	int Status;

	DX_SCREEN()
	{
		Default();
	}
	inline void Default()
	{
		this->DxSurf = NULL;
		this->H_size = 0;
		this->V_size = 0;
		this->Status = 0;
	}

	inline CRect GetRect()
	{
		CRect reRet(0, 0, this->H_size, this->V_size);
		return reRet;
	}
};

#endif // _DX_DEFS_H_