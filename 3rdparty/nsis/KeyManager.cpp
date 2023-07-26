// KeyManager.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "KeyManager.h"

#include "our_md5.h"

static BOOL g_bSelectSchema[6][32] =
{
	{
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
	},
	{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
	},
	{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1
	},
	{
	1, 0, 1, 0, 1, 0, 1, 0,
	1, 0, 1, 0, 1, 0, 1, 0,
	1, 0, 1, 0, 1, 0, 1, 0,
	1, 0, 1, 0, 1, 0, 1, 0
	},
	{
	1, 1, 1, 1, 0, 0, 0, 0,
	1, 1, 1, 1, 0, 0, 0, 0,
	1, 1, 1, 1, 0, 0, 0, 0,
	1, 1, 1, 1, 0, 0, 0, 0
	},
	{
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1
	}
};
static int g_WeightShecma[19] = {0x1,0x1,0x3,0x3,0x5,0x5,0x7,0x7,0x9,0x9,0xb,0xb,0xd,0xd,0xf,0xf,0x1,0x2,0x3};

KeyManager::KeyManager()
{
}
KeyManager::~KeyManager()
{
}

void KeyManager::MD5String(uint8* src, uint8* dst, int len)
{
	MD5_CTX ctx;
	our_MD5Init(&ctx);
	ourMD5Update(&ctx, src, len);
	our_MD5Final(dst, &ctx);
}

int KeyManager::HexString2Int(WCHAR* szHexString)
{
	int nValue = 0;
	int nMultiplier = 1;

	for(int i = (int)wcslen(szHexString) - 1; i >= 0 ; i--)
	{
		if(szHexString[i] >= L'0' && szHexString[i] <= L'9')
			nValue += ((szHexString[i] - L'0') * nMultiplier);
		else if(szHexString[i] >= L'A' && szHexString[i] <= L'F')
			nValue += ((szHexString[i] - L'A' + 10) * nMultiplier);
		else if(szHexString[i] >= L'a' && szHexString[i] <= L'f')
			nValue += ((szHexString[i] - L'a' + 10) * nMultiplier);
		else  return 0;

		nMultiplier = nMultiplier * 16;
	}
	return nValue;
}
BOOL KeyManager::ReadRegString(HKEY hKey, WCHAR* szSubKey, WCHAR* szValue)
{
	DWORD dwType = REG_SZ;
	DWORD dwBufSize = 0;
	long result = RegQueryValueExW( hKey, szSubKey, 0, &dwType, NULL, &dwBufSize);

	if(ERROR_SUCCESS == result)
	{
		result = RegQueryValueExW(
			hKey,
			szSubKey,
			0,
			&dwType,
			(BYTE*)szValue,
			&dwBufSize);
	}
	return (ERROR_SUCCESS == result);
}
BOOL KeyManager::WriteRegString(HKEY hKey, WCHAR* szSubKey, WCHAR* szValue, DWORD dwBufSize)
{
	DWORD dwType  = REG_SZ;
	DWORD result  = RegSetValueExW( hKey, szSubKey, 0, dwType, (const BYTE*)szValue, dwBufSize);

	return (result == ERROR_SUCCESS);
}

BOOL KeyManager::ReadInfo1(LICENSE_INFO1* pInfo)
{
	HKEY hKey = NULL;
	WCHAR szStringValue[256]; memset(szStringValue, 0, sizeof(szStringValue));

	LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT1, 0 , KEY_ALL_ACCESS , &hKey);
	if(ERROR_SUCCESS != result) return FALSE;

	if(ReadRegString(hKey, (WCHAR*)L"SerialNumber", szStringValue))
	{
		pInfo->wzNumber = szStringValue;
	}
	if(ReadRegString(hKey, (WCHAR*)L"Key", szStringValue))
	{
		pInfo->wzKey = szStringValue;
	}
	if(ReadRegString(hKey, (WCHAR*)L"Install Date", szStringValue))
	{
		pInfo->wzInstDate = szStringValue;
	}

	RegCloseKey(hKey);
	hKey = NULL;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 1);
	pInfo->TrialType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 19, 1);
	pInfo->ExpireType = HexString2Int(szTemp);

	return TRUE;
}
BOOL KeyManager::AnalysisInfo1(LICENSE_INFO1* pInfo)
{
	if(pInfo->wzKey == 0)
		return FALSE;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 1);
	pInfo->TrialType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 19, 1);
	pInfo->ExpireType = HexString2Int(szTemp);

	return TRUE;
}
BOOL KeyManager::WriteInfo1(LICENSE_INFO1* pInfo)
{
	if(pInfo->wzNumber == 0)
		return FALSE;
	if(pInfo->wzKey == 0)
		return FALSE;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 1);
	pInfo->TrialType = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 19, 1);
	pInfo->ExpireType = HexString2Int(szTemp);

	wchr256 wzKey;
	wzKey = pInfo->wzKey;

	if(!MakeSerialKey1(pInfo))
		return FALSE;
	if(wzKey != pInfo->wzKey)
		return FALSE;

	HKEY hKey = NULL;
	SYSTEMTIME stTime; memset(&stTime, 0, sizeof(stTime));

	GetLocalTime(&stTime);
	swprintf_s(pInfo->wzInstDate.sz,
		L"%04d-%02d-%02d %02d:%02d:%02d",
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);

	LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT1, 0 , KEY_ALL_ACCESS , &hKey);
	if(ERROR_SUCCESS != result)
	{
		DWORD dwDisPos = 0;
		result = RegCreateKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT1, 0 , NULL,  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisPos);
		if(ERROR_SUCCESS != result)
		{
			if(hKey) RegCloseKey(hKey);
			return FALSE;
		}
	}

	DWORD bufSize = 0;

	bufSize = ((DWORD)wcslen(pInfo->wzNumber.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"SerialNumber", pInfo->wzNumber.sz, bufSize);

	bufSize = ((DWORD)wcslen(pInfo->wzKey.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"Key", pInfo->wzKey.sz, bufSize);

	bufSize = ((DWORD)wcslen(pInfo->wzInstDate.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"Install Date", pInfo->wzInstDate.sz, bufSize);

	RegCloseKey(hKey);
	hKey = NULL;

	return TRUE;
}
BOOL KeyManager::MakeSerialKey1(LICENSE_INFO1* pInfo)
{
	if(pInfo->wzNumber == 0)
		return FALSE;

	switch(pInfo->KeyType)
	{
	case KEY_TYPE_TRIAL:
	case KEY_TYPE_16CH:
	case KEY_TYPE_36CH:
	case KEY_TYPE_64CH:
	case KEY_TYPE_UNLIMITED: break;
	default: return FALSE;
	}
	switch(pInfo->TrialType)
	{
	case TRIAL_TYPE_DEFUALT:
	case TRIAL_TYPE_EXPIRE_16:
	case TRIAL_TYPE_EXPIRE_36:
	case TRIAL_TYPE_EXPIRE_64:
	case TRIAL_TYPE_EXPIRE_UNLIMITED: break;
	default: return FALSE;
	}
	switch(pInfo->ExpireType)
	{
	case EXPIRE_TYPE_UNLIMITED:
	case EXPIRE_TYPE_30DAYS:
	case EXPIRE_TYPE_60DAYS:
	case EXPIRE_TYPE_90DAYS: break;
	default: return FALSE;
	}

	uint8 md5Key[64]; memset(md5Key, 0, sizeof(md5Key));
	char serialKey[64]; memset(serialKey, 0, sizeof(serialKey));
	char hexString[64]; memset(hexString, 0, sizeof(hexString));
	char hexStringTemp[64]; memset(hexStringTemp, 0, sizeof(hexStringTemp));

	char cz256[256];
	_cmn_w2a(pInfo->wzNumber.sz, cz256, 256);
	MD5String((uint8*)cz256, md5Key, (int)strlen(cz256));

	for(int i=0; i<16; i++)
	{
		sprintf_s(hexStringTemp, "%02X", md5Key[i]);
		strcat_s(hexString, hexStringTemp);
	}

	int index = 4;
	switch(pInfo->KeyType)
	{
	case KEY_TYPE_TRIAL: index = 0; break;
	case KEY_TYPE_16CH: index = 1; break;
	case KEY_TYPE_36CH: index = 2; break;
	case KEY_TYPE_64CH: index = 3; break;
	case KEY_TYPE_UNLIMITED: index = 4; break;
	}

	for(int i=0, j=0; i<32; i++)
	{
		if(g_bSelectSchema[index][i])
		{
			serialKey[j] = hexString[i];
			j++;
		}
	}

	sprintf_s(hexStringTemp, "%02X", pInfo->KeyType);
	strcat_s(serialKey, hexStringTemp);

	sprintf_s(hexStringTemp, "%01X", pInfo->TrialType);
	strcat_s(serialKey, hexStringTemp);

	sprintf_s(hexStringTemp, "%01X", pInfo->ExpireType);
	strcat_s(serialKey, hexStringTemp);

	int check = 0;
	for(int i=0; i<20; i++)
	{
		check += serialKey[i] * g_WeightShecma[(i % 19)];
	}
	check = check % 0xffff;

	sprintf_s(hexStringTemp, "%04X", check);
	strcat_s(serialKey, hexStringTemp);

	pInfo->wzKey = wz256_a2w(serialKey).sz;

	return TRUE;
}

BOOL KeyManager::ReadInfo2(LICENSE_INFO2* pInfo)
{
	HKEY hKey = NULL;
	WCHAR szStringValue[256]; memset(szStringValue, 0, sizeof(szStringValue));

	LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT2, 0 , KEY_ALL_ACCESS , &hKey);
	if(ERROR_SUCCESS != result) return FALSE;

	if(ReadRegString(hKey, (WCHAR*)L"SerialNumber", szStringValue))
	{
		pInfo->wzNumber = szStringValue;
	}
	if(ReadRegString(hKey, (WCHAR*)L"Key", szStringValue))
	{
		pInfo->wzKey = szStringValue;
	}
	if(ReadRegString(hKey, (WCHAR*)L"Install Date", szStringValue))
	{
		pInfo->wzInstDate = szStringValue;
	}

	RegCloseKey(hKey);
	hKey = NULL;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyNumber = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 2);
	pInfo->ExpireType = HexString2Int(szTemp);

	return TRUE;
}
BOOL KeyManager::AnalysisInfo2(LICENSE_INFO2* pInfo)
{
	if(pInfo->wzKey == 0)
		return FALSE;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyNumber = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 2);
	pInfo->ExpireType = HexString2Int(szTemp);

	return TRUE;
}
BOOL KeyManager::WriteInfo2(LICENSE_INFO2* pInfo)
{
	if(pInfo->wzNumber == 0)
		return FALSE;
	if(pInfo->wzKey == 0)
		return FALSE;

	WCHAR szTemp[32] = {0, };

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 16, 2);
	pInfo->KeyNumber = HexString2Int(szTemp);

	memset(szTemp, 0, sizeof(szTemp));
	wcsncpy_s(szTemp, pInfo->wzKey.sz + 18, 2);
	pInfo->ExpireType = HexString2Int(szTemp);

	wchr256 wzKey;
	wzKey = pInfo->wzKey;

	if(!MakeSerialKey2(pInfo))
		return FALSE;
	if(wzKey != pInfo->wzKey)
		return FALSE;

	HKEY hKey = NULL;
	SYSTEMTIME stTime; memset(&stTime, 0, sizeof(stTime));

	GetLocalTime(&stTime);
	swprintf_s(pInfo->wzInstDate.sz,
		L"%04d-%02d-%02d %02d:%02d:%02d",
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);

	LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT2, 0 , KEY_ALL_ACCESS , &hKey);
	if(ERROR_SUCCESS != result)
	{
		DWORD dwDisPos = 0;
		result = RegCreateKeyExW(HKEY_CURRENT_USER, LICENSE_KEY_ROOT2, 0 , NULL,  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisPos);
		if(ERROR_SUCCESS != result)
		{
			if(hKey) RegCloseKey(hKey);
			return FALSE;
		}
	}

	DWORD bufSize = 0;

	bufSize = ((DWORD)wcslen(pInfo->wzNumber.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"SerialNumber", pInfo->wzNumber.sz, bufSize);

	bufSize = ((DWORD)wcslen(pInfo->wzKey.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"Key", pInfo->wzKey.sz, bufSize);

	bufSize = ((DWORD)wcslen(pInfo->wzInstDate.sz) + 1) * 2;
	WriteRegString(hKey, (WCHAR*)L"Install Date", pInfo->wzInstDate.sz, bufSize);

	RegCloseKey(hKey);
	hKey = NULL;

	return TRUE;
}
BOOL KeyManager::MakeSerialKey2(LICENSE_INFO2* pInfo)
{
	if(pInfo->wzNumber == 0)
		return FALSE;

	uint8 md5Key[64]; memset(md5Key, 0, sizeof(md5Key));
	char serialKey[64]; memset(serialKey, 0, sizeof(serialKey));
	char hexString[64]; memset(hexString, 0, sizeof(hexString));
	char hexStringTemp[64]; memset(hexStringTemp, 0, sizeof(hexStringTemp));

	char cz256[256];
	_cmn_w2a(pInfo->wzNumber.sz, cz256, 256);
	MD5String((uint8*)cz256, md5Key, (int)strlen(cz256));

	for(int i=0; i<16; i++)
	{
		sprintf_s(hexStringTemp, "%02X", md5Key[i]);
		strcat_s(hexString, hexStringTemp);
	}

	int index = 4;
	for(int i=0, j=0; i<32; i++)
	{
		if(g_bSelectSchema[index][i])
		{
			serialKey[j] = hexString[i];
			j++;
		}
	}

	sprintf_s(hexStringTemp, "%02X", pInfo->KeyNumber);
	strcat_s(serialKey, hexStringTemp);

	sprintf_s(hexStringTemp, "%02X", pInfo->ExpireType);
	strcat_s(serialKey, hexStringTemp);

	int check = 0;
	for(int i=0; i<20; i++)
	{
		check += serialKey[i] * g_WeightShecma[(i % 19)];
	}
	check = check % 0xffff;

	sprintf_s(hexStringTemp, "%04X", check);
	strcat_s(serialKey, hexStringTemp);

	pInfo->wzKey = wz256_a2w(serialKey).sz;

	return TRUE;
}
