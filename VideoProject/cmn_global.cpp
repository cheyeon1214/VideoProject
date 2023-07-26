// cmn_global.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "cmn_global.h"

/*****************************************************************************
 *
 *  cmn_global
 *
 ****************************************************************************/
cmn_global::cmn_global()
{
	WCHAR szPgmDir[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szPgmDir, _MAX_PATH); 

	CString Path = szPgmDir;
	int index = Path.ReverseFind(L'\\');
	swprintf_s(m_szMyPath, L"%s", (LPCTSTR)Path.Left(index));
	if( m_szMyPath[0] >= L'a' && m_szMyPath[0] <= L'z' )
		m_szMyPath[0] -= (L'a' - L'A');
	::SetCurrentDirectory(m_szMyPath);//<----------------

	swprintf_s(m_szResPath, L"%s\\res", m_szMyPath);
	swprintf_s(m_szImgPath, L"%s\\img", m_szResPath);
	swprintf_s(m_szIcoPath, L"%s\\ico", m_szResPath);
	swprintf_s(m_szWavPath, L"%s\\wav", m_szResPath);

	m_Lang = ID_English;
	usLangID = 0x409;
}
cmn_global::~cmn_global()
{
}

WCHAR * cmn_global::_MyPath()
{
	return m_szMyPath;
}
WCHAR * cmn_global::_ResPath()
{
	return m_szResPath;
}
WCHAR * cmn_global::_ImgPath()
{
	return m_szImgPath;
}
WCHAR * cmn_global::_IcoPath()
{
	return m_szIcoPath;
}
WCHAR * cmn_global::_WavPath()
{
	return m_szWavPath;
}

UINT cmn_global::_GetLang()
{
	return m_Lang;
}
void cmn_global::_SetLang(UINT uiLang)
{
	m_Lang = uiLang;
	switch(uiLang)
	{
	case ID_English:
		usLangID = 0x409;
		break;
	case ID_Korean:
		usLangID = 0x412;
		break;
	default:
		usLangID = 0x409;
		break;
	}
}
BOOL cmn_global::_GetLang(UINT nIDS, WCHAR* szLang, int nLen)
{
	CString str;
	if(!str.LoadString(AfxGetInstanceHandle(), nIDS, usLangID))
		if(!str.LoadString(AfxGetInstanceHandle(), nIDS, 0x409))
			return FALSE;
	swprintf_s(szLang, nLen, L"%s", (LPCTSTR)str);
	return TRUE;
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

cmn_global g_cmn;

WCHAR* _cmn_MyPath()
{
	return g_cmn._MyPath();
}
WCHAR* _cmn_ResPath()
{
	return g_cmn._ResPath();
}
WCHAR* _cmn_ImgPath()
{
	return g_cmn._ImgPath();
}
WCHAR* _cmn_IcoPath()
{
	return g_cmn._IcoPath();
}
WCHAR* _cmn_WavPath()
{
	return g_cmn._WavPath();
}

UINT _lang_Get()
{
	return g_cmn._GetLang();
}
void _lang_Set(UINT uiLang)
{
	g_cmn._SetLang(uiLang);
}
BOOL _lang_(UINT nIDL, WCHAR* szLang, int nLen)
{
	return g_cmn._GetLang(nIDL, szLang, nLen);
}

static int LockMgr(void** mutex, enum AVLockOp op)
{
	switch(op)
	{
	case AV_LOCK_CREATE:
	{
		HANDLE hHandle = NULL;
		hHandle = CreateMutex(NULL, FALSE, NULL);
		if(hHandle)
		{
			*mutex = hHandle;
		}
		return (hHandle == NULL);
	}
	break;
	case AV_LOCK_DESTROY:
	{
		HANDLE hHandle = *mutex;
		if(hHandle) CloseHandle(hHandle);
		return 0;
	}
	break;
	case AV_LOCK_OBTAIN:
	{
		HANDLE hHandle = *mutex;
		if(hHandle) WaitForSingleObject(hHandle, INFINITE);
	}
	break;
	case AV_LOCK_RELEASE:
	{
		HANDLE hHandle = *mutex;
		if(hHandle) ReleaseMutex(hHandle);
	}
	break;
	default:
		return -1; // invalid option
	}

	return 0;
}
BOOL _codec_ffmpeg_Init()
{
	avcodec_register_all();

	if(av_lockmgr_register(&LockMgr))
		return FALSE;
	return TRUE;
}
void _codec_ffmpeg_Exit()
{
	av_lockmgr_register(NULL);
}
