#ifndef _DEFS_TRACE_H_
#define _DEFS_TRACE_H_

inline void _wchr_Trace(const WCHAR * fmt, ...)
{
	HWND np = FindWindow(L"Notepad", NULL);
	if(np != NULL)
	{
		HWND ed = FindWindowEx(np, NULL, L"Edit", NULL);
		if(ed != NULL)
		{
			WCHAR buf[1024];

			va_list args;
			va_start(args, fmt);
			long buf_cnt = _vsnwprintf_s(buf, (sizeof(buf)/sizeof(buf[0])), fmt, args);
			::SendMessageW(ed, EM_REPLACESEL, (WPARAM)0, (LPARAM)buf);
			va_end(args);
		}
	}
}

inline void _char_Trace(const char * fmt, ...)
{
	HWND np = FindWindow(L"Notepad", NULL);
	if(np != NULL)
	{
		HWND ed = FindWindowEx(np, NULL, L"Edit", NULL);
		if(ed != NULL)
		{
			char buf[1024];

			va_list args;
			va_start(args, fmt);
			long buf_cnt = _vsnprintf_s(buf, (sizeof(buf)/sizeof(buf[0])), fmt, args);
			::SendMessageA(ed, EM_REPLACESEL, (WPARAM)0, (LPARAM)buf);
			va_end(args);
		}
	}
}

inline void _char_long_Trace(char * sz)
{
	char* buf = sz;
	int len = (int)strlen(sz);

	char disp[512];
	while(len > 0)
	{
		if(len > 500){
			memcpy(disp, buf, 500);
			disp[500] = '\0';
			buf += 500; len -= 500;
			_char_Trace("%s", disp);
		}
		else{
			memcpy(disp, buf, len);
			disp[len] = '\0';
			len = 0;
			_char_Trace("%s\r\n", disp);
		}
	}
}

#endif // _DEFS_TRACE_H_
