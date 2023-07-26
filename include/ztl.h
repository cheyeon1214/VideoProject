// -*- C++ -*-

#if !defined(_ztl_h)	// Sentry, use file only if it's not already included.
#define _ztl_h

#if defined(WIN32)

#include <process.h>

class ZSemaphore
{
	HANDLE m_hSemaph;

public:
	ZSemaphore (long lMaxCount = 1, long lInitialCount = -1)
	{
		if (lInitialCount < 0)
			lInitialCount = lMaxCount;
		m_hSemaph = CreateSemaphore (NULL, lInitialCount, lMaxCount, NULL);
	}
	~ZSemaphore () {CloseHandle (m_hSemaph);}
	DWORD P (UINT uTimeout = INFINITE)
		{return WaitForSingleObject (m_hSemaph, uTimeout);}
	BOOL V () {return ReleaseSemaphore (m_hSemaph, 1, NULL);}
	BOOL Try () {return WaitForSingleObject (m_hSemaph, 0) != WAIT_TIMEOUT;}
};

class ZMutex
{
	HANDLE m_hMutex;

public:
	ZMutex () {m_hMutex = CreateMutex (NULL, FALSE, NULL);}
	~ZMutex () {CloseHandle (m_hMutex);}
	DWORD P (UINT uTimeout = INFINITE)
		{return WaitForSingleObject (m_hMutex, uTimeout);}
	BOOL V () {return ReleaseMutex (m_hMutex);}
	BOOL Try () {return WaitForSingleObject (m_hMutex, 0) != WAIT_TIMEOUT;}
};

class ZEvent
{
	HANDLE m_hEvent;

public:
	ZEvent () {m_hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);}
	~ZEvent () {CloseHandle (m_hEvent);}
	DWORD P (UINT uTimeout = 1000)
	{
		return WaitForSingleObject (m_hEvent, uTimeout);
	}
	BOOL V () {return SetEvent (m_hEvent);}
	BOOL Reset () {return ResetEvent (m_hEvent);}
};

class ZCriticalSection
{
	CRITICAL_SECTION m_hCritSec;

public:
	ZCriticalSection () { InitializeCriticalSection(&m_hCritSec); }
	~ZCriticalSection () { DeleteCriticalSection(&m_hCritSec); }
	void P ()
		{ EnterCriticalSection(&m_hCritSec); }
	BOOL T ()
		{ return TryEnterCriticalSection(&m_hCritSec); }
	void V () { LeaveCriticalSection(&m_hCritSec); }
};

class ZCsLock
{
	ZCriticalSection* m_pCs;

public:
	ZCsLock (ZCriticalSection* pLock) { m_pCs = pLock; if(m_pCs) m_pCs->P(); }
	~ZCsLock () { if(m_pCs) m_pCs->V(); }
};

class T_Event
{
	HANDLE m_hEvent;
	HANDLE m_hThread;

public:
	T_Event ()
	{
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThread = NULL;
	}
	~T_Event ()
	{
		CloseHandle(m_hEvent);
	}

	BOOL T_Active()
	{
		if(m_hThread != NULL)
			return TRUE;
		return FALSE;
	}
	void T_End(BOOL bError = FALSE)
	{
		m_hThread = NULL;
		if(!bError) SetEvent(m_hEvent);
	}
	void T_Begin(void(*address)(void *), void * arg)
	{
		ResetEvent(m_hEvent);
		m_hThread = (HANDLE)_beginthread(address,0,arg);
	}
	void T_Terminate(UINT uTimeout = 1000)
	{
		if(m_hThread != NULL)
		{
			WaitForSingleObject(m_hEvent, uTimeout);
			if(m_hThread != NULL)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hThread, &ExitCode))
				{
					TerminateThread(m_hThread, ExitCode);
					CloseHandle(m_hThread);
				}
				m_hThread = NULL;
			}
		}
	}
};

#endif

#endif                                      // _ztl_h sentry.
