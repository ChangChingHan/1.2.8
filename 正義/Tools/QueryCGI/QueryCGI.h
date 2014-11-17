
// QueryCGI.h : main header file for the PROJECT_NAME application
//
#include "QueryManager.h"
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CQueryCGIApp:
// See QueryCGI.cpp for the implementation of this class
//

class CQueryCGIApp : public CWinAppEx
{
public:
	CQueryCGIApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	QueryManager *m_qm;
};

extern CQueryCGIApp theApp;