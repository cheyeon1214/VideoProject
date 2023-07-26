// Dx3D9.cpp: implementation of the Dx3D9 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dx3D9.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Dx3D9::Dx3D9()
{
	m_hWnd = NULL;

	m_rgb_cx = 0; m_rgb_cy = 0;

	m_lpD3D = NULL;
	m_lpD3DD = NULL;
	m_lpRGBback = NULL;
	m_lpRGBsnap = NULL;
	for(int i=0; i<MAX_IMG; i++)
	{
		m_lpImg[i] = NULL;
	}
}
Dx3D9::~Dx3D9()
{
}

BOOL Dx3D9::Init3D9()
{
	return TRUE;
}
void Dx3D9::Exit3D9()
{
	for(int i=0; i<MAX_IMG; i++)
	{
		SAFE_RELEASE(m_lpImg[i]);
	}
	SAFE_RELEASE(m_lpRGBsnap);
	SAFE_RELEASE(m_lpRGBback);
	SAFE_RELEASE(m_lpD3DD);
	SAFE_RELEASE(m_lpD3D);
}

BOOL Dx3D9::InitD3DDevice()
{
	SAFE_RELEASE(m_lpD3DD);

	D3DCAPS9 deviceCaps;
	HRESULT hr = m_lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps);
	if(FAILED(hr)) return FALSE;

	DWORD behaviorFlag = D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
	behaviorFlag |= (deviceCaps.VertexProcessingCaps != 0) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	memset(&m_d3dpp, 0, sizeof(m_d3dpp));
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.hDeviceWindow = m_hWnd;
	m_d3dpp.BackBufferCount  = 1;
	m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferWidth  = m_rgb_cx + 16;
	m_d3dpp.BackBufferHeight = m_rgb_cy + 16;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.Flags                      = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;//(if use back->GetDC)
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	hr = m_lpD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
		behaviorFlag,
		&m_d3dpp,
		&m_lpD3DD);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

BOOL Dx3D9::InitRGBBackSurface()
{
	SAFE_RELEASE(m_lpRGBback);

	m_d3dpp.BackBufferWidth  = m_rgb_cx + 16;
	m_d3dpp.BackBufferHeight = m_rgb_cy + 16;

	HRESULT hr = m_lpD3DD->Reset(&m_d3dpp);
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3DD->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3DD->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_lpRGBback);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

BOOL Dx3D9::Init_StsImgSurface()
{
	for(int i=0; i<MAX_IMG; i++)
	{
		SAFE_RELEASE(m_lpImg[i]);
		memset(&m_infImg[i], 0, sizeof(D3DXIMAGE_INFO));

		WCHAR szImgPath[MAX_PATH];
		swprintf_s(szImgPath, L"%s\\%s", _cmn_ImgPath(), szImgName[i]);

		HRESULT hr = D3DXGetImageInfoFromFile(szImgPath, &m_infImg[i]);
		if(FAILED(hr)) return FALSE;

		hr = m_lpD3DD->CreateOffscreenPlainSurface(
			m_infImg[i].Width, m_infImg[i].Height,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&(m_lpImg[i]),
			0);
		if(FAILED(hr)) return FALSE;

		hr = D3DXLoadSurfaceFromFile(m_lpImg[i], NULL, NULL, szImgPath, NULL, D3DX_DEFAULT, 0, NULL);
		if(FAILED(hr)) return FALSE;
	}
	return TRUE;
}
