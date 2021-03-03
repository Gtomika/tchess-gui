// TChessPromotionDialog.cpp : implementation file
//

#include "pch.h"
#include "TChessGUI.h"
#include "TChessPromotionDialog.h"
#include "afxdialogex.h"

#include "board/board.h"

// TChessPromotionDialog dialog

IMPLEMENT_DYNAMIC(TChessPromotionDialog, CDialogEx)

TChessPromotionDialog::TChessPromotionDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROMOTION, pParent), promotedPiece(tchess::queen)
{

}

TChessPromotionDialog::~TChessPromotionDialog()
{
}

void TChessPromotionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROM_QUEEN, promQueenRadioButton);
}

BOOL TChessPromotionDialog::OnInitDialog()
{
	//check queen as defaul promotion
	promQueenRadioButton.SetCheck(true);
	return 0;
}


BEGIN_MESSAGE_MAP(TChessPromotionDialog, CDialogEx)
	ON_BN_CLICKED(IDC_PROM_QUEEN, &TChessPromotionDialog::OnBnClickedPromQueen)
	ON_BN_CLICKED(IDC_PROM_ROOK, &TChessPromotionDialog::OnBnClickedPromRook)
	ON_BN_CLICKED(IDC_PROM_BISHOP, &TChessPromotionDialog::OnBnClickedPromBishop)
	ON_BN_CLICKED(IDC_PROM_KNIGHT, &TChessPromotionDialog::OnBnClickedPromKnight)
END_MESSAGE_MAP()


// TChessPromotionDialog message handlers


void TChessPromotionDialog::OnBnClickedPromQueen()
{
	promotedPiece = tchess::queen;
}


void TChessPromotionDialog::OnBnClickedPromRook()
{
	promotedPiece = tchess::rook;
}


void TChessPromotionDialog::OnBnClickedPromBishop()
{
	promotedPiece = tchess::bishop;
}


void TChessPromotionDialog::OnBnClickedPromKnight()
{
	promotedPiece = tchess::knight;
}
