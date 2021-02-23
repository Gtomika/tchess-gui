// TChessRootDialogView.cpp : implementation file
//

#include <iostream>

#include "pch.h"
#include "TChessGUI.h"
#include "TChessRootDialogView.h"

#include "engine/engine.h"
#include "greedy/greedy_player.h"
#include "random/random_player.h"


// TChessRootDialogView

IMPLEMENT_DYNCREATE(TChessRootDialogView, CFormView)

TChessRootDialogView::TChessRootDialogView()
	: CFormView(IDD_ROOT_DIALOG_VIEW)
{
	gameObject = nullptr;
}

TChessRootDialogView::~TChessRootDialogView()
{
}

void TChessRootDialogView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	//initialize all views here
}

void TChessRootDialogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WHITE_SELECT, comboWhiteSelector);
	DDX_Control(pDX, IDC_COMBO_BLACK_SELECT, comboBlackSelector);
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
tchess::player* playerFromText(const CString& text, unsigned int side) {
	if (text.Compare(_T("Player (user controlled)"))) {
		//TODO: create human player GUI
		return nullptr;
	}
	else if (text.Compare(_T("TChess engine"))) {
		return new tchess::engine(side);
	}
	else if (text.Compare(_T("Greedy move maker"))) {
		return new tchess::greedy_player(side);
	}
	else { //must be random
		return new tchess::random_player(side);
	}
}

// TChessRootDialogView message handlers

void TChessRootDialogView::OnBnClickedStartGameButton()
{
	//extract text from combo boxes
	CString whiteSelected;
	comboWhiteSelector.GetLBText(comboWhiteSelector.GetCurSel(), whiteSelected);
	CString blackSelected;
	comboBlackSelector.GetLBText(comboBlackSelector.GetCurSel(), blackSelected);
	if (whiteSelected.IsEmpty() || blackSelected.IsEmpty()) {
		//players not selected
		AfxMessageBox(_T("Players must be selected!"), MB_OK | MB_ICONSTOP);
		return;
	}
	//both players were selected
	tchess::player* whitePlayer = playerFromText(whiteSelected, 0);
	tchess::player* blackPlayer = playerFromText(blackSelected, 1);
	//create game object
	gameObject = new tchess::game(whitePlayer, blackPlayer);
	//TODO: start game
}
