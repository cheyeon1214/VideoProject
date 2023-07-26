#ifndef _DXS_CONTROL_H_
#define _DXS_CONTROL_H_

#include "Dxs3D9.h"

class dxs_control
{
private:
	CWnd * m_pWnd;
	Dxs3D9 * m_pD3d;

	BOOL m_bActive;

public:
	BOOL Init(CWnd * pWnd);
	void Exit();
	CWnd* GetWnd(){return m_pWnd;}
	BOOL DxCheck();
	void* GetD3DD();

	DX_SCREEN* GetDxScrn();
	CRect GetScreen();

	int GetStatus();

	void ScreenSplit();
	void ScreenCover();
	void ChangeSplit();

	void SendImageSrc(VID_IMGSRC* pVid_img);
	void SendCleanMsg(int STATUS);
	void Send_Refresh();

public:
	dxs_control();
	~dxs_control();
};

#endif
