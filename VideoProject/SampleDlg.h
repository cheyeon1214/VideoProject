#pragma once

// CSampleDlg ��ȭ �����Դϴ�.

class CSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

public:
	CSampleDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSampleDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHILD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
