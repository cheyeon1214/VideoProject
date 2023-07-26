/********************************************************************/
/*																	*/
/*  IconButton.cpp													*/
/*																	*/
/*  Implementation of the CIconButton.								*/
/*																	*/
/*	This class make it possible to display an icon and text on a	*/
/*	button, which unfortunately can't be done with CButton...		*/
/*																	*/
/*	CIconButton also has support for XP themes, so you can have		*/
/*	themed buttons that have an icon!								*/
/*																	*/
/*  Programmed by Pablo van der Meer								*/
/*  Copyright Pablo Software Solutions 2002							*/
/*	http://www.pablovandermeer.nl									*/
/*																	*/
/*  Last updated: 19 September 2002									*/
/*																	*/
/********************************************************************/

#include "pch.h"
#include "IconButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CIconButton::CIconButton()
{
	m_Icon = NULL;
	m_Checked = m_button_down = m_tracking = false;
}

CIconButton::~CIconButton()
{
	if(m_Icon) DeleteObject(m_Icon);
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(BM_SETCHECK , OnBMSetCheck)
	ON_MESSAGE(BM_GETCHECK , OnBMGetCheck)
END_MESSAGE_MAP()


/********************************************************************/
/*																	*/
/* Function name : DrawItem											*/
/* Description   : Called by the framework when a visual aspect of	*/
/*				   an owner-draw button changes.					*/
/*																	*/
/********************************************************************/
void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	UINT uStyle = DFCS_BUTTONPUSH;

	// if button is pushed, add the pushed style to DrawFrameControl
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) || m_Checked)
		uStyle |= DFCS_PUSHED;

	// is button flat ?
	if (GetButtonStyle() & BS_FLAT)
		uStyle |= DFCS_FLAT;

	// clear background
	pDC->FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_BTNFACE));

	// XP theme support
	if (m_Theme.IsThemingSupported())
	{
		int	nStateId = 0;
		if (m_Theme.OpenThemeData(GetSafeHwnd(), L"BUTTON"))
		{
			nStateId = PBS_NORMAL;

			// unfortunately an ownerdrawn button can't also have the BS_DEFPUSHBUTTON style
//			if (lpDrawItemStruct->itemState & ODS_DEFAULT)
//				nStateId = PBS_DEFAULTED;

			if (lpDrawItemStruct->itemState & ODS_FOCUS)
				nStateId = PBS_HOT;
			if ((lpDrawItemStruct->itemState & ODS_SELECTED) || m_Checked)
				nStateId = PBS_PRESSED;
			if (lpDrawItemStruct->itemState & ODS_DISABLED)
				nStateId = PBS_DISABLED;

			m_Theme.DrawThemeBackground(pDC->GetSafeHdc(), BP_PUSHBUTTON, nStateId, &lpDrawItemStruct->rcItem);

			m_Theme.CloseThemeData();
		}
		else
		{
			// draw the button frame
			::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
		}
	}
	else
	{
		// draw the button frame
		::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
	}

	// get button text
	CString strText;
	GetWindowText(strText);

	// set default button colors
	pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
	pDC->SetBkMode(TRANSPARENT);

	// is button disabled ?
	BOOL bDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CRect rcIcon(0,0,0,0);
	// icon loaded ?
	if(m_Icon)
	{
		// calculate icon's rectangle
		rcIcon = lpDrawItemStruct->rcItem;
		CalcIconRect(pDC, rcIcon);

		CPoint pt = rcIcon.TopLeft();

		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			pt.Offset(1, 1);

		// draw icon
		pDC->DrawState(pt, CSize(m_nIconWidth, m_nIconHeight),
			(HICON)m_Icon, DST_ICON | (bDisabled ? DSS_DISABLED : DSS_NORMAL), (CBrush *)NULL);
	}
	else
	{
		// if no icon is loaded, just display text
	}

	// calculate text's rectangle
	CRect rcText = lpDrawItemStruct->rcItem;
	CalcTextRect(pDC, strText, rcText, rcIcon);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		rcText.OffsetRect(1, 1);

	// draw text
	pDC->DrawState(rcText.TopLeft(), rcText.Size(), (LPCTSTR)strText, (bDisabled ? DSS_DISABLED : DSS_NORMAL), TRUE, 0, (CBrush*)NULL);

	// and finally draw the focus rectangle
	if(0)//lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		rcText = lpDrawItemStruct->rcItem;
		::InflateRect(&rcText, -2 * ::GetSystemMetrics(SM_CXEDGE), -2 * ::GetSystemMetrics(SM_CYEDGE));
		pDC->DrawFocusRect(&rcText);
	}
}


/********************************************************************/
/*																	*/
/* Function name : PreSubclassWindow								*/
/* Description   : Initialize some stuff							*/
/*																	*/
/********************************************************************/
void CIconButton::PreSubclassWindow() 
{
	m_Style=GetButtonStyle();	///get specific BS_ styles
	if((m_Style & BS_AUTOCHECKBOX)==BS_AUTOCHECKBOX)
		 m_Style=BS_CHECKBOX;
	else m_Style=BS_PUSHBUTTON;

	// set default style
	ModifyStyle(NULL, BS_OWNERDRAW | BS_ICON | BS_VCENTER | BS_LEFT);
	CButton::PreSubclassWindow();
}


/********************************************************************/
/*																	*/
/* Function name : SetIcon											*/
/* Description   : Set button's icon								*/
/*																	*/
/********************************************************************/
BOOL CIconButton::SetIcon(LPWSTR strIcon, int cx, int cy)
{
	m_Icon = (HICON)::LoadImage(AfxGetInstanceHandle(),
								 strIcon,
								 IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
	if(m_Icon == NULL) return FALSE;

	ICONINFO iinfo;
	GetIconInfo(m_Icon, &iinfo);

	m_nIconWidth = iinfo.xHotspot * 2;
	m_nIconHeight = iinfo.yHotspot * 2;

	return TRUE;
}
BOOL CIconButton::SetIcon(WCHAR* szPath, WCHAR* szIcon, int cx, int cy)
{
	WCHAR szIconPath[MAX_PATH];
	swprintf_s(szIconPath, L"%s\\%s", szPath, szIcon);

	m_Icon = (HICON)::LoadImage(AfxGetInstanceHandle(),
								 szIconPath,
								 IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
	if(m_Icon == NULL) return FALSE;

	ICONINFO iinfo;
	GetIconInfo(m_Icon, &iinfo);

	m_nIconWidth = iinfo.xHotspot * 2;
	m_nIconHeight = iinfo.yHotspot * 2;

	return TRUE;
}
BOOL CIconButton::SetIconAuto(WCHAR* szPath, WCHAR* szIcon)
{
	CRect re;
	GetClientRect(&re);

	int cx = 16, cy = 16;
	if(re.Height() > 48){cx = 48; cy = 48;}
	else if(re.Height() > 32){cx = 32; cy = 32;}

	WCHAR szIconPath[MAX_PATH];
	swprintf_s(szIconPath, L"%s\\%s", szPath, szIcon);

	m_Icon = (HICON)::LoadImage(AfxGetInstanceHandle(),
								 szIconPath,
								 IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
	if(m_Icon == NULL) return FALSE;

	ICONINFO iinfo;
	GetIconInfo(m_Icon, &iinfo);

	m_nIconWidth = iinfo.xHotspot * 2;
	m_nIconHeight = iinfo.yHotspot * 2;

	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : CalcIconRect										*/
/* Description   : Calculate icon's rectangle depending on button	*/
/*				   style.											*/
/*																	*/
/********************************************************************/
BOOL CIconButton::CalcIconRect(CDC *pDC, CRect &rcIcon)
{
	DWORD dwStyle = GetStyle();
	CRect rc = rcIcon;

	// calculate icon rectangle 
	rc.right  = rc.left + m_nIconWidth;
	rc.bottom = rc.top  + m_nIconHeight;

	int nOffset;
	switch(dwStyle & (BS_CENTER|BS_RIGHT))
	{
		case BS_LEFT:    
			rc.left += 5;  
			rc.right += 5;  
			break;

		case BS_CENTER:  
			nOffset = rc.right - rc.left;
            rc.left = rcIcon.left + ((rcIcon.right - rcIcon.left) - nOffset) / 2;
            rc.right = rc.left + nOffset; 
			break;
		
		case BS_RIGHT:   
			nOffset = rc.right - rc.left;
            rc.right = rcIcon.right - 5;
            rc.left = rc.right - nOffset;
            break;
	}

   	switch (dwStyle & (BS_VCENTER|BS_BOTTOM))
	{
		case BS_TOP:     
			rc.top += 4;  
			rc.bottom += 4;  
			break;
		
		case BS_VCENTER: 
			nOffset = rc.bottom - rc.top;
            rc.top = rcIcon.top + ((rcIcon.bottom - rcIcon.top) - nOffset) / 2;
            rc.bottom = rc.top + nOffset;
			break;
		
		case BS_BOTTOM:  
			nOffset = rc.bottom - rc.top;
            rc.bottom = rcIcon.bottom - 4;
            rc.top = rc.bottom - nOffset;
            break;
	}
	rcIcon = rc;
	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : CalcTextRect										*/
/* Description   : Calculate text's rectangle depending on button	*/
/*				   style.											*/
/*																	*/
/********************************************************************/
BOOL CIconButton::CalcTextRect(CDC *pDC, LPCTSTR lpszText, CRect &rcText, CRect &rcIcon)
{
	DWORD dwStyle = GetStyle();

	CRect rc = rcText;

	switch(dwStyle & (BS_CENTER|BS_RIGHT))
	{
		case BS_LEFT:    
			rc.left = rcIcon.right;  
			break;

		case BS_CENTER:  
			rc.top = rcIcon.bottom;
			break;
		
		case BS_RIGHT:   
			rc.right = rcIcon.left;
            break;
	}

	rcText = rc;

	// calculate text size	
	pDC->DrawText(lpszText, -1, &rc, DT_SINGLELINE | DT_CALCRECT);

   	switch (dwStyle & (BS_VCENTER|BS_BOTTOM))
	{
		case BS_TOP:     
			rc.OffsetRect((rcText.Width() - rc.Width())/2, rcIcon.top + rcIcon.Height()/2 - rc.Height()/2);
			break;
		
		case BS_VCENTER: 
			rc.OffsetRect((rcText.Width() - rc.Width())/2, (rcText.Height() - rc.Height())/2);
			rc.OffsetRect(0, -1);
			break;
		
		case BS_BOTTOM:  
			rc.OffsetRect((rcText.Width() - rc.Width())/2, rcIcon.top + rcIcon.Height()/2 - rc.Height()/2);
			break;
	}

	rcText = rc;
	return TRUE;
}

void CIconButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
}

void CIconButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	if (m_tracking) {
        TRACKMOUSEEVENT t = {
            sizeof(TRACKMOUSEEVENT),
            TME_CANCEL | TME_LEAVE,
            m_hWnd, 0
		};
        if (::_TrackMouseEvent(&t)) {
            m_tracking = false;
        }
    }

	CButton::OnLButtonDown(nFlags, point);
    m_button_down = true;
}
void CIconButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Style){ //track mouse for radio & check buttons
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);
		if (mouse_wnd == m_hWnd && m_button_down == true){
			if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
		}
	}

	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    m_button_down = false;
	CButton::OnLButtonUp(nFlags, point);
}
void CIconButton::OnMouseMove(UINT nFlags, CPoint point)
{
	RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    if ((m_button_down) && (::GetCapture() == m_hWnd)) {
	    POINT p2 = point;
        ::ClientToScreen(m_hWnd, &p2);
        HWND mouse_wnd = ::WindowFromPoint(p2);

        bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
        bool need_pressed = (mouse_wnd == m_hWnd);
        if (pressed != need_pressed) {
            SetState(need_pressed ? TRUE : FALSE);
            Invalidate();
        }
    } else {
        if (!m_tracking) {
            TRACKMOUSEEVENT t = {
                sizeof(TRACKMOUSEEVENT),
                TME_LEAVE,
                m_hWnd,
                0
            };
            if (::_TrackMouseEvent(&t)) {
                m_tracking = true;
                Invalidate();
            }
        }
    }

    CButton::OnMouseMove(nFlags, point);
}
LRESULT CIconButton::OnMouseLeave(WPARAM, LPARAM)
{
    m_tracking = false;
    Invalidate();
	return 0;
}

LRESULT CIconButton::OnBMSetCheck(WPARAM wparam, LPARAM)
{
	m_Checked=wparam!=0;
	switch (m_Style)
	{
	case BS_PUSHBUTTON:
		m_Checked=false;
	}
	Invalidate();
	return 0;
}
LRESULT CIconButton::OnBMGetCheck(WPARAM wparam, LPARAM)
{ return m_Checked; }
