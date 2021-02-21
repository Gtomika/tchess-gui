
// TChessGUI.h : main header file for the TChessGUI application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTChessGUIApp:
// See TChessGUI.cpp for the implementation of this class
//

class CTChessGUIApp : public CWinApp
{
public:
	CTChessGUIApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTChessGUIApp theApp;
