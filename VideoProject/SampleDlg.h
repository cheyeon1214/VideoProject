#pragma once

// CSampleDlg 대화 상자입니다.

class CSampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

public:
	CSampleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSampleDlg();

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
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
