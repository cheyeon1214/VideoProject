#pragma once

// CClientDlg 대화 상자입니다.

#include "dxs_control_exe.h"

#include "H264DEC.h"

class CVideoProjectDlg;
class CClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CClientDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

private:
	dxs_control * m_pdx;

	BOOL m_bStart;
	uint8* m_bufVid;
	H264DEC* m_decVid;

	SOCKET m_sockCnt;
	char m_szIP[64];
	int m_iPort;

public:
	CVideoProjectDlg* m_pMainDlg;
	BOOL Create(CVideoProjectDlg* pParent);

	void CLOSE();
	BOOL ACCEPT(SOCKET sdAccept);
	BOOL CONNECT(WCHAR* szIP, int iPort);
	void SendMpgDatSrc(AVS_PACKET* pAVS_dat);

private:
	friend static void ChildThread(void* Ptr);
	void ChildThread();

	BOOL m_bChild;
	HANDLE m_hChildThread;
	HANDLE m_hChildThreadEnd;

private:
	friend static void ClientThread(void* Ptr);
	void ClientThread();

	BOOL m_bClient;
	HANDLE m_hClientThread;
	HANDLE m_hClientThreadEnd;

public:
	CClientDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClientDlg();

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
