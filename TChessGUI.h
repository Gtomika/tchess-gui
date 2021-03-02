
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
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSourceViewCodeOnGithub();
	afx_msg void OnSourceViewConsoleVersionOnGithub();
	afx_msg void OnHelpHowToStartGame();
	afx_msg void OnHelpHowToMakeMove();
};

extern CTChessGUIApp theApp;
