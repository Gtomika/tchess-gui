#pragma once


// TChessPromotionDialog dialog

class TChessPromotionDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TChessPromotionDialog)

public:
	TChessPromotionDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~TChessPromotionDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROMOTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	//Stores what the user selected to promote to. Queen by default.
	unsigned int promotedPiece;

	afx_msg void OnBnClickedPromQueen();
	afx_msg void OnBnClickedPromRook();
	afx_msg void OnBnClickedPromBishop();
	afx_msg void OnBnClickedPromKnight();
	CButton promQueenRadioButton;
};
