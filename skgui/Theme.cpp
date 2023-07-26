/********************************************************************/
/*																	*/
/*  Theme.cpp														*/
/*																	*/
/*  Implementation of the CTheme class.								*/
/*																	*/
/*	This class is just a simple MFC wrapper for XP theme support.	*/
/*																	*/
/*  Programmed by Pablo van der Meer								*/
/*  Copyright Pablo Software Solutions 2002							*/
/*	http://www.pablovandermeer.nl									*/
/*																	*/
/*  Last updated: 19 September 2002									*/
/*																	*/
/********************************************************************/

#include "pch.h"
#include "Theme.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTheme::CTheme()
{
	m_hTheme = NULL;
	IsThemingSupported();
}

CTheme::~CTheme()
{

}
