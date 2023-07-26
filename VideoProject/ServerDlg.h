#pragma once

// CServerDlg 대화 상자입니다.

#include "dxs_control_exe.h"

#include "VinputControl.h"
#include "H264ENC.h"

class CVideoProjectDlg;
class CServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CServerDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	ConfigSvr m_cfg;

private:
	dxs_control* m_pdx;

	VinputControl* m_pCamDrv;

	uint8* m_bufVid;
	H264ENC* m_encVid;

	SOCKET m_sockSvr;

public:
	CVideoProjectDlg* m_pMainDlg;
	BOOL Create(CVideoProjectDlg* pParent);

	void SendRawImgSrc(VID_IMGSRC* pVid_img);
	void CLOSE();

	BOOL SetCfg(ConfigSvr* pCfg);
	ConfigSvr* GetCfg() { return &m_cfg; }

private:
	friend static void AcceptThread(void* Ptr);
	void AcceptThread();

	BOOL m_bAcpt;
	HANDLE m_hAcptThread;
	HANDLE m_hAcptThreadEnd;

public:
	CServerDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHILD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
