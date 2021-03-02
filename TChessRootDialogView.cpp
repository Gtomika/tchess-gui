// TChessRootDialogView.cpp : implementation file
//

#include <iostream>

#include "pch.h"

#include "TChessGUI.h"
#include "TChessRootDialogView.h"

#include "image_utils.h"
#include "board/board.h"
#include "board/move.h"
#include "engine/engine.h"
#include "greedy/greedy_player.h"
#include "random/random_player.h"
#include "human/human_player_gui.h"

#define NUM_SQUARES 64

// TChessRootDialogView

const UINT MOVE_CALCULATED_MESSAGE = RegisterWindowMessage(_T("MOVE_CALCULATED_MESSAGE"));

//The message type that is sent when a player agent determines how long it needs to calculate a move.
const UINT MOVE_GENERATION_RANGE = RegisterWindowMessage(_T("MOVE_GENERATION_RANGE"));

//The message type that is sent when move calculation progresses.
const UINT MOVE_GENERATION_PROGRESS = RegisterWindowMessage(_T("MOVE_GENERATION_PROGRESS"));

IMPLEMENT_DYNCREATE(TChessRootDialogView, CFormView)

TChessRootDialogView::TChessRootDialogView()
	: CFormView(IDD_ROOT_DIALOG_VIEW)
{
	gameObject = nullptr;
	squareControls = std::vector<CPictureCtrl>(NUM_SQUARES);
	squareFrom = -1;
	squareTo = -1;
}

TChessRootDialogView::~TChessRootDialogView()
{
}

void TChessRootDialogView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	//initialize all views here

	//set engine difficulty radio button
	radioDiff6.SetCheck(true);

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
	DDX_Control(pDX, IDC_MOVE_GENERATION_STATUS, moveGenerationStatusText);
	DDX_Control(pDX, IDC_MOVE_GENERATION_PROGRESS, moveGenerationProgress);
	DDX_Control(pDX, IDC_RESIGN_BUTTON, resignButton);
	DDX_Control(pDX, IDC_DIFFICULTY_2, radioDiff2);
	DDX_Control(pDX, IDC_DEFFICULTY_3, radioDiff3);
	DDX_Control(pDX, IDC_DIFFICULTY_4, radioDiff4);
	DDX_Control(pDX, IDC_DIFFICULTY_5, radioDiff5);
	DDX_Control(pDX, IDC_DIFFICULTY_6, radioDiff6);
}

BEGIN_MESSAGE_MAP(TChessRootDialogView, CFormView)
	ON_BN_CLICKED(IDC_START_GAME_BUTTON, &TChessRootDialogView::OnBnClickedStartGameButton)
	ON_BN_CLICKED(IDC_MAKE_MOVE, &TChessRootDialogView::OnBnClickedMakeMove)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SQUARE_0, IDC_SQUARE_63, &TChessRootDialogView::OnSquareClicked) //register click event on all squares
	ON_REGISTERED_MESSAGE(MOVE_CALCULATED_MESSAGE, &TChessRootDialogView::OnMoveCalculated) //add message handles functions
	ON_REGISTERED_MESSAGE(MOVE_GENERATION_PROGRESS, &TChessRootDialogView::OnMoveGenerationProgressed)
	ON_REGISTERED_MESSAGE(MOVE_GENERATION_RANGE, &TChessRootDialogView::OnMoveGenerationRangeFound)
	ON_BN_CLICKED(IDC_RESIGN_BUTTON, &TChessRootDialogView::OnBnClickedResignButton)
	ON_BN_CLICKED(IDC_DIFFICULTY_2, &TChessRootDialogView::OnBnClickedDifficulty2)
	ON_BN_CLICKED(IDC_DEFFICULTY_3, &TChessRootDialogView::OnBnClickedDefficulty3)
	ON_BN_CLICKED(IDC_DIFFICULTY_4, &TChessRootDialogView::OnBnClickedDifficulty4)
	ON_BN_CLICKED(IDC_DIFFICULTY_5, &TChessRootDialogView::OnBnClickedDifficulty5)
	ON_BN_CLICKED(IDC_DIFFICULTY_6, &TChessRootDialogView::OnBnClickedDifficulty6)
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
	moveGenerationStatusText.SetWindowText(_T("Moves generation not in progress"));
	tchess::move* m = reinterpret_cast<tchess::move*>(lp); //cast lp to move pointer
	gameObject->submitMove(*m); //give move to the controller
	delete m;
	return LRESULT();
}

//Called when move generation is progressed. LPARAM is the new progress amount.
LRESULT TChessRootDialogView::OnMoveGenerationProgressed(WPARAM wp, LPARAM lp)
{
	short* progress = reinterpret_cast<short*>(lp);
	moveGenerationProgress.SetPos(*progress);
	delete progress;
	return LRESULT();
}

//Called when move generation length is determined by the player. WPARAM is range bottom, LPARAM is range top.
LRESULT TChessRootDialogView::OnMoveGenerationRangeFound(WPARAM wp, LPARAM lp)
{
	short* bottom = reinterpret_cast<short*>(wp);
	short* top = reinterpret_cast<short*>(lp);
	moveGenerationProgress.SetRange(*bottom, *top);
	delete bottom;
	delete top;
	return LRESULT();
}

void TChessRootDialogView::OnSquareClicked(UINT squareId)
{
	if (gameObject == nullptr) return; //do nothing if game is not ongoing
	if (!gameObject->gameOngoing()) return;
	if (!gameObject->isAwaitingGui()) return; //do nothing if it is not a GUI interactive players turn to move
	//get square controller
	CStatic* squareControl = (CStatic*)GetDlgItem(squareId);
	//get the number of the square
	UINT squareNumber = squareId - IDC_SQUARE_0;
	if (squareFrom == -1) {
		//selecting starting square
		squareFrom = squareNumber;
	}
	else {
		//selecting destination square or cancelling starting square
		if (squareFrom == squareNumber) {
			//cancelled
			squareFrom = -1;
		}
		else {
			//actual destination square selected
			squareTo = squareNumber;
			//move selected, try to parse: using move parser for this one
			try {
				char pieceCode = tchess::pieceNameFromCode(std::abs(gameObject->getBoard()[squareFrom]));
				char promCode = NO_PROMOTION;
				//check if this move is a promotion, if yes, ask a promotion piece with a dialog: TODO
				tchess::move parsedMove = tchess::parse_move(pieceCode, squareFrom, squareTo, gameObject->getInfo().getSideToMove(), promCode);
				gameObject->submitMove(parsedMove);
				squareFrom = -1;
				squareTo = -1;
			}
			catch (tchess::move_parse_exception& e) {
				//failed to parse
				squareFrom = -1;
				squareTo = -1;
				CString message = CString(e.what());
				AfxMessageBox(message, MB_OK | MB_ICONERROR);
			}
		}
	}
}

void TChessRootDialogView::OnBnClickedResignButton()
{
	if (gameObject == nullptr) return; //do nothing if game is not ongoing
	if (!gameObject->gameOngoing()) return;
	if (!gameObject->isAwaitingGui()) return; //do nothing if it is not a GUI interactive players turn to move
	//ask for confirmation
	int decision = AfxMessageBox(_T("Are you sure you want to resign?"), MB_YESNO | MB_ICONQUESTION);
	if (decision == IDYES) {
		//submit a resign move
		tchess::move resignMove = tchess::move(0, 0, tchess::resignMove, 0);
		gameObject->submitMove(resignMove);
	}
}


void TChessRootDialogView::OnBnClickedDifficulty2()
{
	tchess::engine_depth = 2;
}


void TChessRootDialogView::OnBnClickedDefficulty3()
{
	tchess::engine_depth = 3;
}


void TChessRootDialogView::OnBnClickedDifficulty4()
{
	tchess::engine_depth = 4;
}


void TChessRootDialogView::OnBnClickedDifficulty5()
{
	tchess::engine_depth = 5;
}


void TChessRootDialogView::OnBnClickedDifficulty6()
{
	tchess::engine_depth = 6;
}
