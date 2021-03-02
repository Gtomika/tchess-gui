
// TChessGUI.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "TChessGUI.h"
#include "MainFrm.h"

#include "TChessGUIDoc.h"
#include "TChessGUIView.h"
#include "TChessRootDialogView.h"

#include "engine/engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTChessGUIApp construction

CTChessGUIApp::CTChessGUIApp() noexcept
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("TChessGUI.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CTChessGUIApp object

CTChessGUIApp theApp;


// CTChessGUIApp initialization

BOOL CTChessGUIApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("TChess GUI Application"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTChessGUIDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(TChessRootDialogView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->SetWindowPos(&m_pMainWnd->wndTop, 200, 200, 1200, 800, SWP_NOSIZE);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CTChessGUIApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CTChessGUIApp message handlers

BEGIN_MESSAGE_MAP(CTChessGUIApp, CWinApp)
	ON_COMMAND(ID_SOURCE_VIEWCODEONGITHUB, &CTChessGUIApp::OnSourceViewCodeOnGithub)
	ON_COMMAND(ID_SOURCE_VIEWCONSOLEVERSIONONGITHUB, &CTChessGUIApp::OnSourceViewConsoleVersionOnGithub)
	ON_COMMAND(ID_HELP_HOWTOSTARTAGAME, &CTChessGUIApp::OnHelpHowToStartGame)
	ON_COMMAND(ID_HELP_HOWTOMAKEAMOVE, &CTChessGUIApp::OnHelpHowToMakeMove)
END_MESSAGE_MAP()

// CTChessGUIApp message handlers

void CTChessGUIApp::OnSourceViewCodeOnGithub()
{
	ShellExecute(0, 0, _T("https://github.com/Gtomika/tchess-gui"), 0, 0, SW_SHOW);
}


void CTChessGUIApp::OnSourceViewConsoleVersionOnGithub()
{
	ShellExecute(0, 0, _T("https://github.com/Gtomika/tchess"), 0, 0, SW_SHOW);
}


void CTChessGUIApp::OnHelpHowToStartGame()
{
	AfxMessageBox(_T("Select two players on the left, then click start game."), MB_OK | MB_ICONINFORMATION);
}


void CTChessGUIApp::OnHelpHowToMakeMove()
{
	AfxMessageBox(_T("When it's your turn to move, click on the piece you want to move, \
		then on the square you wish to move it. If you changed your mind, click in the piece again to cancel moving that."), MB_OK | MB_ICONINFORMATION);
}
