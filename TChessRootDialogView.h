#pragma once

#ifndef TCHESS_ROOT_DIALOG_VIEW_H_
#define TCHESS_ROOT_DIALOG_VIEW_H_

#include <afxext.h>
#include <afxcmn.h>

#include <vector>

#include "game/game.h"
#include "game/saved_game.h"
#include "game/player.h"
#include "PictureCtrl.h"

namespace tchess
{
	class game;
	class saved_game;
}

// TChessRootDialogView form view

//The message type that is sent when a move calculation finishes.
extern const UINT MOVE_CALCULATED_MESSAGE;

//The message type that is sent when a player agent determines how long it needs to calculate a move.
extern const UINT MOVE_GENERATION_RANGE;

//The message type that is sent when move calculation progresses.
extern const UINT MOVE_GENERATION_PROGRESS;

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
	void createSaveGame();

	//Called when the start game button is clicked.
	afx_msg void OnBnClickedStartGameButton();
	//Called when the make move button is clicked.
	afx_msg void OnBnClickedMakeMove();
	//Called when the move calculator background thread finished the work
	afx_msg LRESULT OnMoveCalculated(WPARAM wp, LPARAM lp);
	//Called when move generation progresses.
	afx_msg LRESULT OnMoveGenerationProgressed(WPARAM wp, LPARAM lp);
	//Called when move generation length is determined by the player.
	afx_msg LRESULT OnMoveGenerationRangeFound(WPARAM wp, LPARAM lp);
	//Called when a square is clicked. Id of the square is the parameter
	afx_msg void OnSquareClicked(UINT squareId);
	//Called when the resign button is clicked.
	afx_msg void OnBnClickedResignButton();
	//Engine difficulty radio button event handlers
	afx_msg void OnBnClickedDifficulty2();
	afx_msg void OnBnClickedDefficulty3();
	afx_msg void OnBnClickedDifficulty4();
	afx_msg void OnBnClickedDifficulty5();
	afx_msg void OnBnClickedDifficulty6();

	//Pointer to the game object, or null if no game is ongoing
	tchess::game* gameObject;
	//Pointer to the saved game, if there is one
	tchess::saved_game* savedGame;

	//Starting square clicked on the GUI. -1 if none was clicked so far.
	UINT squareFrom;
	//Destination square clicked on the GUI -1 if none was clicked so far.
	UINT squareTo;

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
	// Control variable for the move generation status text.
	CStatic moveGenerationStatusText;
	// Control variable for the move generation progress.
	CProgressCtrl moveGenerationProgress;
	
	// Control variable for the resign button.
	CButton resignButton;
	//Controls for engine difficulty radio buttons
	CButton radioDiff2;
	CButton radioDiff3;
	CButton radioDiff4;
	CButton radioDiff5;
	CButton radioDiff6;
	// Control variable for the list view displaying the moves of the game.
	CListCtrl moveList;
	afx_msg void OnGameSaveToFile();
	afx_msg void OnGameLoadFromFile();
};

#endif

