#ifndef _DIALOG_DEFS_H_
#define _DIALOG_DEFS_H_

struct ConfigSvr
{
	BOOL bUseCam;
	int index;
	int hsize, vsize;

	BOOL bUseSvr;
	WCHAR szIP[64]; int iPort;

	ConfigSvr()
	{
		this->bUseCam = FALSE;
		this->index = 0;
		this->hsize = 640;
		this->vsize = 480;

		this->bUseSvr = FALSE;
		wcscpy_s(this->szIP, L"127.0.0.1");
		this->iPort = 12000;
	}
	inline ConfigSvr& operator = (ConfigSvr& other)
	{
		this->bUseCam = other.bUseCam;
		this->index = other.index;
		this->hsize = other.hsize;
		this->vsize = other.vsize;

		this->bUseSvr = other.bUseSvr;
		wcscpy_s(this->szIP, other.szIP);
		this->iPort = other.iPort;

		return *this;
	}
	inline BOOL operator == (ConfigSvr& other)
	{
		if(this->bUseCam != other.bUseCam) return FALSE;
		if(this->index != other.index) return FALSE;
		if(this->hsize != other.hsize) return FALSE;
		if(this->vsize != other.vsize) return FALSE;

		if(this->bUseSvr != other.bUseSvr) return FALSE;
		if(wcscmp(this->szIP, other.szIP)) return FALSE;
		if(this->iPort != other.iPort) return FALSE;

		return TRUE;
	}
	inline BOOL operator != (ConfigSvr& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
};

#endif // _DIALOG_DEFS_H_
