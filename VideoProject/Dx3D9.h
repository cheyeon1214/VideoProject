// Dx3D9.h: interface for the Dx3D9 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _DX_3D9_H_
#define  _DX_3D9_H_

class Dx3D9
{
protected:
	HWND m_hWnd;

	int m_rgb_cx, m_rgb_cy;

protected:
	IDirect3D9* m_lpD3D;
	IDirect3DDevice9* m_lpD3DD;
	IDirect3DSurface9* m_lpRGBback;
	IDirect3DSurface9* m_lpRGBsnap;
	IDirect3DSurface9* m_lpImg[MAX_IMG];

	D3DPRESENT_PARAMETERS m_d3dpp;
	D3DXIMAGE_INFO m_infImg[MAX_IMG];

	BOOL InitD3DDevice();
	BOOL InitRGBBackSurface();
	BOOL Init_StsImgSurface();

protected:
	BOOL Init3D9();
	void Exit3D9();

public:
	Dx3D9::Dx3D9();
	Dx3D9::~Dx3D9();
};

#endif // _DXS3D9_H_
