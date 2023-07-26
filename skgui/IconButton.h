#if !defined(AFX_ICONBUTTON_H__5D3CEAAA_ADE5_46B5_B1D9_56FE69DB361D__INCLUDED_)
#define AFX_ICONBUTTON_H__5D3CEAAA_ADE5_46B5_B1D9_56FE69DB361D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconButton.h : header file
//

#include "Theme.h"

class CIconButton : public CButton
{
// Attributes
protected:
	HICON m_Icon;
	int m_nIconWidth;
	int m_nIconHeight;

	BOOL CalcIconRect(CDC *pDC, CRect &rcIcon);
	BOOL CalcTextRect(CDC *pDC, LPCTSTR lpszText, CRect &rcText, CRect &rcIcon);

	CTheme m_Theme;

	bool	m_Checked;
	DWORD	m_Style;
	bool	m_tracking;
	bool	m_button_down;

	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

	// Operations
public:
	BOOL SetIcon(LPWSTR strIcon, int cx = 0, int cy = 0);
	BOOL SetIcon(WCHAR* szPath, WCHAR* szIcon, int cx = 0, int cy = 0);
	BOOL SetIconAuto(WCHAR* szPath, WCHAR* szIcon);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
	CIconButton();
	virtual ~CIconButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBUTTON_H__5D3CEAAA_ADE5_46B5_B1D9_56FE69DB361D__INCLUDED_)
