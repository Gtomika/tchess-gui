// TChessRootDialogView.cpp : implementation file
//

#include <iostream>

#include "pch.h"

#include "TChessGUI.h"
#include "TChessRootDialogView.h"

#include "image_utils.h"
#include "board/board.h"

#include "engine/engine.h"
#include "greedy/greedy_player.h"
#include "random/random_player.h"
#include "human/human_player_gui.h"

#define NUM_SQUARES 64

// TChessRootDialogView

const UINT MOVE_CALCULATED_MESSAGE = RegisterWindowMessage(_T("MOVE_CALCULATED_MESSAGE"));

IMPLEMENT_DYNCREATE(TChessRootDialogView, CFormView)

TChessRootDialogView::TChessRootDialogView()
	: CFormView(IDD_ROOT_DIALOG_VIEW)
{
	gameObject = nullptr;
	squareControls = std::vector<CPictureCtrl>(NUM_SQUARES);
}

TChessRootDialogView::~TChessRootDialogView()
{
}

void TChessRootDialogView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	//initialize all views here

	//draw a starter board so squares are not completely empty
	tchess::chessboard starterBoard;
	tchess::drawBoard(starterBoard, squareControls);
}

void TChessRootDialogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WHITE_SELECT, comboWhiteSelector);
	DDX_Control(pDX, IDC_COMBO_BLACK_SELECT, comboBlackSelector);
	for (size_t i = 0; i < NUM_SQUARES; ++i) {
		DDX_Control(pDX, IDC_SQUARE_0 + i, squareControls[i]);
	}
	DDX_Control(pDX, IDC_GAME_STATUS, gameStatus);
	DDX_Control(pDX, IDC_WHITE_NAME, whitePlayerName);
	DDX_Control(pDX, IDC_BLACK_NAME, blackPlayerName);
	DDX_Control(pDX, IDC_TURN_TO_MOVE, turnToMove);
	DDX_Control(pDX, IDC_GAME_RESULT, gameResult);
	DDX_Control(pDX, IDC_GAME_REASON, gameResultReason);
	DDX_Control(pDX, IDC_USE_MAKE_MOVE_BUTTON, useMakeMoveCheckbox);
	DDX_Control(pDX, IDC_MAKE_MOVE, makeMoveButton);
}

BEGIN_MESSAGE_MAP(TChessRootDialogView, CFormView)
	ON_BN_CLICKED(IDC_START_GAME_BUTTON, &TChessRootDialogView::OnBnClickedStartGameButton)
	ON_BN_CLICKED(IDC_MAKE_MOVE, &TChessRootDialogView::OnBnClickedMakeMove)
	ON_REGISTERED_MESSAGE(MOVE_CALCULATED_MESSAGE, &TChessRootDialogView::OnMoveCalculated) 
END_MESSAGE_MAP()


// TChessRootDialogView diagnostics

#ifdef _DEBUG
void TChessRootDialogView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void TChessRootDialogView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

//player selector helper
char TChessRootDialogView::playerFromText(const CString& text) {
	if (text.Compare(_T("Player (user controlled)")) == 0) {
		return tchess::humanPlayerCode;
	}
	else if (text.Compare(_T("TChess engine")) == 0) {
		return tchess::engineCode;
	}
	else if (text.Compare(_T("Greedy move maker")) == 0) {
		return tchess::greedyPlayerCode;
	}
	else { //must be random
		return tchess::randomPlayerCode;
	}
}

// TChessRootDialogView message handlers

void TChessRootDialogView::OnBnClickedStartGameButton()
{
	if (gameObject != nullptr && gameObject->gameOngoing()) {
		//there is an active game. ask confirmation
		int res = AfxMessageBox(_T("A game is in progress. Are you sure you want to start a new one?"), MB_YESNO | MB_ICONWARNING);
		if (res != IDYES) return; //user chose to abort
	}
	//extract text from combo boxes
	CString whiteSelected;
	whiteSelected.Empty();
	comboWhiteSelector.GetLBText(comboWhiteSelector.GetCurSel(), whiteSelected);
	CString blackSelected;
	blackSelected.Empty();
	comboBlackSelector.GetLBText(comboBlackSelector.GetCurSel(), blackSelected);
	if (whiteSelected.IsEmpty() || blackSelected.IsEmpty()) {
		//players not selected
		AfxMessageBox(_T("Players must be selected!"), MB_OK | MB_ICONSTOP);
		return;
	}
	//delete old game if there is one
	if (gameObject != nullptr) delete gameObject;
	//both players were selected
	char whitePlayerCode = playerFromText(whiteSelected);
	char blackPlayerCode = playerFromText(blackSelected);
	//check if we should use 'make move' button
	bool useMakeMove = useMakeMoveCheckbox.GetCheck();
	makeMoveButton.EnableWindow(useMakeMove); //enable or disable button
	//create game object
	gameObject = new tchess::game(whitePlayerCode, blackPlayerCode, this, useMakeMove);
	gameObject->startGame();
}

//called when the make move button is clicked.
void TChessRootDialogView::OnBnClickedMakeMove()
{
	if (gameObject == nullptr) return;
	if (!gameObject->gameOngoing()) return;
	//dont do anything if a move calculation is in progress
	if (gameObject->isAwaitingMove()) return;
	//tell the game controller to make the next move
	gameObject->nextMove();
}

//Called when the move calculator thread finishes. LPARAM is the move pointer
LRESULT TChessRootDialogView::OnMoveCalculated(WPARAM wp, LPARAM lp)
{
	tchess::move* m = reinterpret_cast<tchess::move*>(lp); //cast lp to move pointer
	gameObject->submitMove(*m); //give move to the controller
	return LRESULT();
}
