#pragma once

#include "game/game.h"

#include "PictureCtrl.h"

// TChessRootDialogView form view

class TChessRootDialogView : public CFormView
{
	DECLARE_DYNCREATE(TChessRootDialogView)

protected:
	TChessRootDialogView();           // protected constructor used by dynamic creation
	virtual ~TChessRootDialogView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROOT_DIALOG_VIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	virtual void OnInitialUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedStartGameButton();

	// White player selector combo box
	CComboBox comboWhiteSelector;
	// Black player selector combo box
	CComboBox comboBlackSelector;

	//Player and game related data
	
	//Pointer to the game object, or null if no game is ongoing
	tchess::game* gameObject;
};


