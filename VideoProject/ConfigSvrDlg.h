#pragma once

// CConfigSvrDlg ��ȭ �����Դϴ�.

#include "IconButton.h"

class CConfigSvrDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigSvrDlg)

private:
	ConfigSvr m_cfg;

private:
	BOOL Initialize(BOOL fInitialize);

public:
	void Init(ConfigSvr* pCfg)
	{
		m_cfg = *pCfg;
	}
	ConfigSvr* GetCfg() { return &m_cfg; }

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CButton m_UseCam_btn;
	CEdit m_Index_edt;
	CEdit m_Width_edt;
	CEdit m_Height_edt;

	CButton m_UseSvr_btn;
	CEdit m_IP_edt;
	CEdit m_Port_edt;

	CIconButton m_OK_btn;
	CIconButton m_Cancel_btn;

public:
	CConfigSvrDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CConfigSvrDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONFIG_SVR_DLG };

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
