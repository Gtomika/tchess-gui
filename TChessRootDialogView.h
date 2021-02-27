#pragma once

#ifndef TCHESS_ROOT_DIALOG_VIEW_H_
#define TCHESS_ROOT_DIALOG_VIEW_H_

#include <afxext.h>

#include <vector>

#include "game/game.h"
#include "game/player.h"
#include "PictureCtrl.h"

namespace tchess
{
	class game;
}

// TChessRootDialogView form view

//The message type that is sent when a move calculation finishes.
extern const UINT MOVE_CALCULATED_MESSAGE;

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

private:
	char playerFromText(const CString& text);

public:
	afx_msg void OnBnClickedStartGameButton();
	afx_msg void OnBnClickedMakeMove();

	//Called when the move calculator background thread finished the work
	afx_msg LRESULT OnMoveCalculated(WPARAM wp, LPARAM lp);

	//Pointer to the game object, or null if no game is ongoing
	tchess::game* gameObject;

	// White player selector combo box
	CComboBox comboWhiteSelector;
	// Black player selector combo box
	CComboBox comboBlackSelector;
	//the control variables for all the square views
	std::vector<CPictureCtrl> squareControls;
	//Control variable for the game status text box.
	CStatic gameStatus;
	// Control variable for the white player name display text box.
	CStatic whitePlayerName;
	// Control variable for the black player name display text box.
	CStatic blackPlayerName;
	// Control variable for the text box that displays whose turn it is to move.
	CStatic turnToMove;
	// Control variable for the text box displaying the game result.
	CStatic gameResult;
	// Control variable for the text box displaying the resason why the game ended.
	CStatic gameResultReason;
	// Control variable of the use make move checkbox.
	CButton useMakeMoveCheckbox;
	// Control variable for the make move button.
	CButton makeMoveButton;
	
};

#endif

