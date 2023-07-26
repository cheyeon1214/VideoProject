// KeyManager.h
//

#pragma once

class KeyManager
{
private:
	void MD5String(uint8* src, uint8* dst, int len);

	int	 HexString2Int(WCHAR* szHexString);
	BOOL ReadRegString(HKEY hKey, WCHAR* szSubKey, WCHAR* szValue);
	BOOL WriteRegString(HKEY hKey, WCHAR* szSubKey, WCHAR* szValue, DWORD dwBufSize);

public:
	BOOL ReadInfo1(LICENSE_INFO1* pInfo);
	BOOL WriteInfo1(LICENSE_INFO1* pInfo);
	BOOL AnalysisInfo1(LICENSE_INFO1* pInfo);
	BOOL MakeSerialKey1(LICENSE_INFO1* pInfo);

	BOOL ReadInfo2(LICENSE_INFO2* pInfo);
	BOOL WriteInfo2(LICENSE_INFO2* pInfo);
	BOOL AnalysisInfo2(LICENSE_INFO2* pInfo);
	BOOL MakeSerialKey2(LICENSE_INFO2* pInfo);

public:
	KeyManager();
	~KeyManager();
};
