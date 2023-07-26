
// VideoProjectDlg.h: 헤더 파일
//

#pragma once

#include "ServerDlg.h"
#include "ClientDlg.h"

// CVideoProjectDlg 대화 상자
class CVideoProjectDlg : public CDialog
{
private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	CRect m_re_XY;
	CRect m_re_XY_Svr;
	CRect m_re_XY_Cnt;
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	BOOL ACCEPT(SOCKET sdAccept);
	void SendMpgDatSrc(AVS_PACKET* pAVS_dat);

private:
	CServerDlg m_dlgSvr;
	CClientDlg m_dlgCnt;

private:
	CStatic m_Server_stc;
	CStatic m_Client_stc;

// 생성입니다.
public:
	CVideoProjectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOPROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	HICON m_hIcon2;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
