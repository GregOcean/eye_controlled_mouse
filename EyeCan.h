// mymfc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "cv.h"
#include "highgui.h"
#include "camerads.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>

#define IMAGE_WIDTH 300
#define IMAGE_HEIGHT 226
#define IMAGE_CHANNELS 3

// CmymfcApp:
// See mymfc.cpp for the implementation of this class
//

class CmymfcApp : public CWinApp
{
public:
	CmymfcApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CmymfcApp theApp;