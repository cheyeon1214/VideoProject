#pragma once

// CConnectDlg ��ȭ �����Դϴ�.

#include "IconButton.h"

class CConnectDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectDlg)

private:
	WCHAR m_szIP[64];
	int m_iPort;

private:
	BOOL Initialize(BOOL fInitialize);

public:
	void Init(WCHAR* szIP, int iPort)
	{
		wcscpy_s(m_szIP, szIP);
		m_iPort = iPort;
	}
	WCHAR* GetIP() { return m_szIP; }
	int GetPort() { return m_iPort; }

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CEdit m_IP_edt;
	CEdit m_Port_edt;

	CIconButton m_OK_btn;
	CIconButton m_Cancel_btn;

public:
	CConnectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CConnectDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONNECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnOK();
	afx_msg void OnBnClickedBtnCancel();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
