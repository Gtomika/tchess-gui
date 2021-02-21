
// TChessGUIView.h : interface of the CTChessGUIView class
//

#pragma once


class CTChessGUIView : public CView
{
protected: // create from serialization only
	CTChessGUIView() noexcept;
	DECLARE_DYNCREATE(CTChessGUIView)

// Attributes
public:
	CTChessGUIDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CTChessGUIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TChessGUIView.cpp
inline CTChessGUIDoc* CTChessGUIView::GetDocument() const
   { return reinterpret_cast<CTChessGUIDoc*>(m_pDocument); }
#endif

