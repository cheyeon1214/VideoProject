// Theme.h: interface for the CTheme class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEME_H__CD9E193F_0AEB_4D17_9F1A_325C01DCC91C__INCLUDED_)
#define AFX_THEME_H__CD9E193F_0AEB_4D17_9F1A_325C01DCC91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vssym32.h"
#include "uxtheme.h"
#pragma comment(lib, "uxtheme.lib")

//
// Delay load is NOT AUTOMATIC for VC++ 7, you have to link to delayimp.lib, 
// and add uxtheme.dll in the Linker.Input.Delay Loaded DLLs section of the 
// project properties.
#if (_MSC_VER < 1300) && !defined(_MFC_NO_THEME_DELAYLOAD)
#pragma comment(lib, "delayimp.lib")
#pragma comment(linker, "/delayload:uxtheme.dll")
#endif //(_MSC_VER < 1300) && !defined(_MFC_NO_THEME_DELAYLOAD)


class CTheme  
{
public:
	CTheme();
	virtual ~CTheme();


	// Data members
	HTHEME m_hTheme;
	static int m_nIsThemingSupported;

	// Operators and helpers
	BOOL IsThemeNull() const
	{
		return (m_hTheme == NULL);
	}

	CTheme& operator =(HTHEME hTheme)
	{
		m_hTheme = hTheme;
		return *this;
	}

	operator HTHEME() const
	{
		return m_hTheme;
	}

	void Attach(HTHEME hTheme)
	{
		m_hTheme = hTheme;
	}

	HTHEME Detach()
	{
		HTHEME hTheme = m_hTheme;
		m_hTheme = NULL;
		return hTheme;
	}

	// Theme support helper
	static BOOL IsThemingSupported()
	{
		if(m_nIsThemingSupported == -1)
		{
			if(m_nIsThemingSupported == -1)
			{
				HMODULE hThemeDLL = ::LoadLibrary(_T("uxtheme.dll"));
				m_nIsThemingSupported = (hThemeDLL != NULL) ? 1 : 0;
				if(hThemeDLL != NULL)
					::FreeLibrary(hThemeDLL);
			}
		}

		ASSERT(m_nIsThemingSupported != -1);
		return (m_nIsThemingSupported == 1);
	}

	// Operations and theme properties
	HTHEME OpenThemeData(HWND hWnd, LPCWSTR pszClassList)
	{
		if(!IsThemingSupported())
			return NULL;

		ASSERT(m_hTheme == NULL);
		m_hTheme = ::OpenThemeData(hWnd, pszClassList);
		return m_hTheme;
	}

	HRESULT CloseThemeData()
	{
		HRESULT hRet = S_FALSE;
		if(m_hTheme != NULL)
		{
			hRet = ::CloseThemeData(m_hTheme);
			if(SUCCEEDED(hRet))
				m_hTheme = NULL;
		}
		return hRet;
	}

	HRESULT DrawThemeBackground(HDC hDC, int nPartID, int nStateID, LPCRECT pRect, LPCRECT pClipRect = NULL)
	{
		ASSERT(m_hTheme != NULL);
		return ::DrawThemeBackground(m_hTheme, hDC, nPartID, nStateID, pRect, pClipRect);
	}

	HRESULT DrawThemeText(HDC hDC, int nPartID, int nStateID, LPCWSTR pszText, int nCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
	{
		ASSERT(m_hTheme != NULL);
		return ::DrawThemeText(m_hTheme, hDC, nPartID, nStateID, pszText, nCharCount, dwTextFlags, dwTextFlags2, pRect);
	}

	HRESULT GetThemeBackgroundContentRect(HDC hDC, int nPartID, int nStateID,  LPCRECT pBoundingRect, LPRECT pContentRect) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeBackgroundContentRect(m_hTheme, hDC, nPartID, nStateID,  pBoundingRect, pContentRect);
	}

	HRESULT GetThemeBackgroundExtent(HDC hDC, int nPartID, int nStateID, LPCRECT pContentRect, LPRECT pExtentRect) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeBackgroundExtent(m_hTheme, hDC, nPartID, nStateID, pContentRect, pExtentRect);
	}

	HRESULT GetThemePartSize(HDC hDC, int nPartID, int nStateID, LPRECT pRect, enum THEMESIZE eSize, LPSIZE pSize) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemePartSize(m_hTheme, hDC, nPartID, nStateID, pRect, eSize, pSize);
	}

	HRESULT GetThemeTextExtent(HDC hDC, int nPartID, int nStateID, LPCWSTR pszText, int nCharCount, DWORD dwTextFlags, LPCRECT  pBoundingRect, LPRECT pExtentRect) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeTextExtent(m_hTheme, hDC, nPartID, nStateID, pszText, nCharCount, dwTextFlags, pBoundingRect, pExtentRect);
	}

	HRESULT GetThemeTextMetrics(HDC hDC, int nPartID, int nStateID, PTEXTMETRICW pTextMetric) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeTextMetrics(m_hTheme, hDC, nPartID, nStateID, pTextMetric);
	}

	HRESULT GetThemeBackgroundRegion(HDC hDC, int nPartID, int nStateID, LPCRECT pRect, HRGN* pRegion) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeBackgroundRegion(m_hTheme, hDC, nPartID, nStateID, pRect, pRegion);
	}

	HRESULT HitTestThemeBackground(HDC hDC, int nPartID, int nStateID, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD* pwHitTestCode) const
	{
		ASSERT(m_hTheme != NULL);
		return ::HitTestThemeBackground(m_hTheme, hDC, nPartID, nStateID, dwOptions, pRect, hrgn, ptTest, pwHitTestCode);
	}

	HRESULT DrawThemeEdge(HDC hDC, int nPartID, int nStateID, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect = NULL)
	{
		ASSERT(m_hTheme != NULL);
		return ::DrawThemeEdge(m_hTheme, hDC, nPartID, nStateID, pDestRect, uEdge, uFlags, pContentRect);
	}

	HRESULT DrawThemeIcon(HDC hDC, int nPartID, int nStateID, LPCRECT pRect, HIMAGELIST himl, int nImageIndex)
	{
		ASSERT(m_hTheme != NULL);
		return ::DrawThemeIcon(m_hTheme, hDC, nPartID, nStateID, pRect, himl, nImageIndex);
	}

	BOOL IsThemePartDefined(int nPartID, int nStateID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::IsThemePartDefined(m_hTheme, nPartID, nStateID);
	}

	BOOL IsThemeBackgroundPartiallyTransparent(int nPartID, int nStateID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::IsThemeBackgroundPartiallyTransparent(m_hTheme, nPartID, nStateID);
	}

	HRESULT GetThemeColor(int nPartID, int nStateID, int nPropID, COLORREF* pColor) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeColor(m_hTheme, nPartID, nStateID, nPropID, pColor);
	}

	HRESULT GetThemeMetric(HDC hDC, int nPartID, int nStateID, int nPropID, int* pnVal) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeMetric(m_hTheme, hDC, nPartID, nStateID, nPropID, pnVal);
	}

	HRESULT GetThemeString(int nPartID, int nStateID, int nPropID, LPWSTR pszBuff, int cchMaxBuffChars) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeString(m_hTheme, nPartID, nStateID, nPropID, pszBuff, cchMaxBuffChars);
	}

	HRESULT GetThemeBool(int nPartID, int nStateID, int nPropID, BOOL* pfVal) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeBool(m_hTheme, nPartID, nStateID, nPropID, pfVal);
	}

	HRESULT GetThemeInt(int nPartID, int nStateID, int nPropID, int* pnVal) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeInt(m_hTheme, nPartID, nStateID, nPropID, pnVal);
	}

	HRESULT GetThemeEnumValue(int nPartID, int nStateID, int nPropID, int* pnVal) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeEnumValue(m_hTheme, nPartID, nStateID, nPropID, pnVal);
	}

	HRESULT GetThemePosition(int nPartID, int nStateID, int nPropID, LPPOINT pPoint) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemePosition(m_hTheme, nPartID, nStateID, nPropID, pPoint);
	}

	HRESULT GetThemeFont(int nPartID, HDC hDC, int nStateID, int nPropID, LOGFONTW* pFont) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeFont(m_hTheme, hDC, nPartID, nStateID, nPropID, pFont);
	}

	HRESULT GetThemeRect(int nPartID, int nStateID, int nPropID, LPRECT pRect) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeRect(m_hTheme, nPartID, nStateID, nPropID, pRect);
	}

	HRESULT GetThemeMargins(HDC hDC, int nPartID, int nStateID, int nPropID, LPRECT pRect, PMARGINS pMargins) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeMargins(m_hTheme, hDC, nPartID, nStateID, nPropID, pRect, pMargins);
	}

	HRESULT GetThemeIntList(int nPartID, int nStateID, int nPropID, INTLIST* pIntList) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeIntList(m_hTheme, nPartID, nStateID, nPropID, pIntList);
	}

	HRESULT GetThemePropertyOrigin(int nPartID, int nStateID, int nPropID, enum PROPERTYORIGIN* pOrigin) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemePropertyOrigin(m_hTheme, nPartID, nStateID, nPropID, pOrigin);
	}

	HRESULT GetThemeFilename(int nPartID, int nStateID, int nPropID, LPWSTR pszThemeFileName, int cchMaxBuffChars) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeFilename(m_hTheme, nPartID, nStateID, nPropID, pszThemeFileName, cchMaxBuffChars);
	}

	COLORREF GetThemeSysColor(int nColorID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysColor(m_hTheme, nColorID);
	}

	HBRUSH GetThemeSysColorBrush(int nColorID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysColorBrush(m_hTheme, nColorID);
	}

	int GetThemeSysSize(int nSizeID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysSize(m_hTheme, nSizeID);
	}

	BOOL GetThemeSysBool(int nBoolID) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysBool(m_hTheme, nBoolID);
	}

	HRESULT GetThemeSysFont(int nFontID, LOGFONTW* plf) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysFont(m_hTheme, nFontID, plf);
	}

	HRESULT GetThemeSysString(int nStringID, LPWSTR pszStringBuff, int cchMaxStringChars) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysString(m_hTheme, nStringID, pszStringBuff, cchMaxStringChars);
	}

	HRESULT GetThemeSysInt(int nIntID, int* pnValue) const
	{
		ASSERT(m_hTheme != NULL);
		return ::GetThemeSysInt(m_hTheme, nIntID, pnValue);
	}
};

__declspec(selectany) int CTheme::m_nIsThemingSupported = -1;

#endif // !defined(AFX_THEME_H__CD9E193F_0AEB_4D17_9F1A_325C01DCC91C__INCLUDED_)
