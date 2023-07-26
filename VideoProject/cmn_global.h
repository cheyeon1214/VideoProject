// cmn_global.h
//

#pragma once

class cmn_global
{
private:
	WCHAR m_szMyPath[MAX_PATH];
	WCHAR m_szResPath[MAX_PATH];
	WCHAR m_szImgPath[MAX_PATH];
	WCHAR m_szIcoPath[MAX_PATH];
	WCHAR m_szWavPath[MAX_PATH];

public:
	WCHAR * _MyPath();
	WCHAR * _ResPath();
	WCHAR * _ImgPath();
	WCHAR * _IcoPath();
	WCHAR * _WavPath();

private:
	UINT m_Lang;
	USHORT usLangID;

public:
	UINT _GetLang();
	void _SetLang(UINT uiLang);
	BOOL _GetLang(UINT nIDS, WCHAR* szLang, int nLen);

public:
	cmn_global();
	~cmn_global();
};

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

WCHAR* _cmn_MyPath();
WCHAR* _cmn_ResPath();
WCHAR* _cmn_ImgPath();
WCHAR* _cmn_IcoPath();
WCHAR* _cmn_WavPath();

enum Lang_ID
{
	ID_English = 0,
	ID_Korean,

	ID_LangMax,
};
UINT _lang_Get();
void _lang_Set(UINT uiLang);
BOOL _lang_(UINT nIDS, WCHAR* szLang, int nLen = 64);

BOOL _codec_ffmpeg_Init();
void _codec_ffmpeg_Exit();
