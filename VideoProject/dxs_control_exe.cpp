// dxs_control_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "dxs_control_exe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************************
 *
 *  dxs_control
 *
 ****************************************************************************/
dxs_control::dxs_control()
{
	m_pWnd = NULL;
	m_pD3d = NULL;

	m_bActive = FALSE;
}
dxs_control::~dxs_control()
{
}

BOOL dxs_control::Init(CWnd * pWnd)
{
	m_pWnd = pWnd;
	if(!m_pWnd->GetSafeHwnd()) return FALSE;

	if(m_pD3d==NULL)
	{
		m_pD3d = new Dxs3D9;
		WCHAR * errorString = m_pD3d->Init(pWnd);
		if(errorString)
		{
			delete m_pD3d;
			m_pD3d = NULL;
		}
	}

	m_bActive = TRUE;

	return TRUE;
}
void dxs_control::Exit()
{
	m_bActive = FALSE;

	if(m_pD3d != NULL)
	{
		delete m_pD3d;
		m_pD3d = NULL;
	}
}
BOOL dxs_control::DxCheck()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->DxCheck();
		}
	}
	return TRUE;
}
void* dxs_control::GetD3DD()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->GetD3DD();
		}
	}
	return NULL;
}

DX_SCREEN* dxs_control::GetDxScrn()
{
	DX_SCREEN * pScrn = NULL;
	if(m_pD3d){
		pScrn = m_pD3d->GetDxScrn();
	}
	return pScrn;
}
CRect dxs_control::GetScreen()
{
	CRect reRet;
	m_pWnd->GetClientRect(&reRet);
	return reRet;
}

int dxs_control::GetStatus()
{
	DX_SCREEN * pScrn = GetDxScrn();
	if(pScrn) return pScrn->Status;
	return -1;
}

void dxs_control::ScreenSplit()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->ScreenSplit();
		}
	}
}
void dxs_control::ScreenCover()
{
	if(m_bActive)
	{
		m_bActive = FALSE;
		if(m_pD3d){
			m_pD3d->ClearSurface();
		}
	}
}
void dxs_control::ChangeSplit()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->ChangeSplit();
		}
		Send_Refresh();
	}
}

void dxs_control::SendImageSrc(VID_IMGSRC* pVid_img)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SendImageSrc(pVid_img);
		}
	}
}
void dxs_control::SendCleanMsg(int STATUS)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SendCleanMsg(STATUS);
		}
	}
}
void dxs_control::Send_Refresh()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->Send_Refresh();
		}
	}
}
