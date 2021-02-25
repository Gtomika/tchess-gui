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
}

BEGIN_MESSAGE_MAP(TChessRootDialogView, CFormView)
ON_BN_CLICKED(IDC_START_GAME_BUTTON, &TChessRootDialogView::OnBnClickedStartGameButton)
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
	if (text.Compare(_T("Player (user controlled)"))) {
		return tchess::humanPlayerCode;
	}
	else if (text.Compare(_T("TChess engine"))) {
		return tchess::engineCode;
	}
	else if (text.Compare(_T("Greedy move maker"))) {
		return tchess::greedyPlayerCode;
	}
	else { //must be random
		return tchess::randomPlayerCode;
	}
}

// TChessRootDialogView message handlers

void TChessRootDialogView::OnBnClickedStartGameButton()
{
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
	//create game object
	gameObject = new tchess::game(whitePlayerCode, blackPlayerCode, this);
	//TODO: start game
}
