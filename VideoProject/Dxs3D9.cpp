// Dxs3D9.cpp: implementation of the Dxs3D9 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dxs3D9.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Dxs3D9::Dxs3D9()
{
}
Dxs3D9::~Dxs3D9()
{
	Shutdown();
}

WCHAR * Dxs3D9::Init(CWnd * pWnd)
{
	m_hWnd = pWnd->GetSafeHwnd();

	m_rgb_cx = GetSystemMetrics(SM_CXSCREEN);
	m_rgb_cy = GetSystemMetrics(SM_CYSCREEN);

	static WCHAR errorMessage[200];

	SAFE_RELEASE(m_lpD3D);

	m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_lpD3D)
	{
		wcscpy_s (errorMessage, L"Error in Direct3DCreate9()");
		return (errorMessage);
	}

	if(!InitD3DDevice())
	{
		wcscpy_s (errorMessage, L"Error in InitD3DDevice()");
		return (errorMessage);
	}
	if(!InitRGBBackSurface())
	{
		wcscpy_s (errorMessage, L"Error in InitRGBBackSurface()");
		return (errorMessage);
	}
	if(!Init_StsImgSurface())
	{
		wcscpy_s (errorMessage, L"Error in Init_StsImgSurface()");
		return (errorMessage);
	}

	Init3D9();

	return NULL;
}
void Dxs3D9::Shutdown()
{
	m_Offscrn.Clear();

	Exit3D9();
}
BOOL Dxs3D9::DxCheck()
{
	if(m_lpD3DD)
	{
		HRESULT hr = m_lpD3DD->TestCooperativeLevel();
		if(hr == D3DERR_DEVICELOST)
		{
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			return FALSE;
		}
	}
	return TRUE;
}
void* Dxs3D9::GetD3DD()
{
	return (void*)m_lpD3DD;
}

BOOL Dxs3D9::DrawSCHSurface(LPRECT lpDst, LPRECT lpSrc)
{
	DX_SCREEN * pScrn = GetDxScrn();
	if(!pScrn->DxSurf) return FALSE;
	if(!m_lpRGBback) return FALSE;

	CRect reDst(lpDst);

	HRESULT hr = m_lpD3DD->StretchRect(
		(IDirect3DSurface9*)pScrn->DxSurf, lpSrc,
		m_lpRGBback, &reDst,
		D3DTEXF_NONE
	);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

void Dxs3D9::RenderSurface(LPRECT lpDst, LPRECT lpSrc)
{
	m_lpD3DD->Present(lpSrc, lpDst, NULL, NULL);
}

void Dxs3D9::SendImageSrc(VID_IMGSRC* pVid_img)
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	if(!m_Offscrn.Draw(m_lpD3DD, pVid_img)) return;

	DX_SCREEN * pScrn = GetDxScrn();

	pScrn->DxSurf = m_Offscrn.lpOffSurf;
	pScrn->H_size = m_Offscrn.hsize;
	pScrn->V_size = m_Offscrn.vsize;
	pScrn->Status = SCR_CAM_CONNECT;

	if(!IsWindowVisible(m_hWnd))
		return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(&reDst, &reSrc);

	RenderSurface(&reDst, &reDst);
}
void Dxs3D9::SendCleanMsg(int STATUS)
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	DX_SCREEN * pScrn = GetDxScrn();
	if((pScrn->Status == STATUS) && pScrn->DxSurf) return;

	m_Offscrn.Clear();

	pScrn->Default();
	pScrn->DxSurf = m_lpImg[STATUS];
	pScrn->H_size = m_infImg[STATUS].Width;
	pScrn->V_size = m_infImg[STATUS].Height;
	pScrn->Status = STATUS;

	if(!IsWindowVisible(m_hWnd))
		return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(&reDst, &reSrc);

	RenderSurface(&reDst, &reDst);
}
void Dxs3D9::Send_Refresh()
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	DX_SCREEN * pScrn = GetDxScrn();
	if(!pScrn->DxSurf) return;

	if(!IsWindowVisible(m_hWnd))
		return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(&reDst, &reSrc);

	RenderSurface(&reDst, &reDst);
}

DX_SCREEN* Dxs3D9::GetDxScrn()
{
	return &m_dxScrn;
}

void Dxs3D9::ScreenSplit()
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);
	RenderSurface(&reDst, &reDst);
}
void Dxs3D9::ChangeSplit()
{
	return;

	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);
	if((m_rgb_cx < reDst.Width()) || (m_rgb_cy < reDst.Height()))
	{
		m_rgb_cx = reDst.Width();
		m_rgb_cy = reDst.Height();
	
		InitRGBBackSurface();
	}
}
void Dxs3D9::ClearSurface()
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);
	m_lpD3DD->ColorFill(m_lpRGBback, &reDst, D3DCOLOR_XRGB(0, 0, 0));
}
