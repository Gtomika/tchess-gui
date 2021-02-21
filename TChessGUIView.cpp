
// TChessGUIView.cpp : implementation of the CTChessGUIView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TChessGUI.h"
#endif

#include "TChessGUIDoc.h"
#include "TChessGUIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTChessGUIView

IMPLEMENT_DYNCREATE(CTChessGUIView, CView)

BEGIN_MESSAGE_MAP(CTChessGUIView, CView)
	

END_MESSAGE_MAP()

// CTChessGUIView construction/destruction

CTChessGUIView::CTChessGUIView() noexcept
{
	// TODO: add construction code here

}

CTChessGUIView::~CTChessGUIView()
{
}

BOOL CTChessGUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTChessGUIView drawing

void CTChessGUIView::OnDraw(CDC* /*pDC*/)
{
	CTChessGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

// CTChessGUIView diagnostics

#ifdef _DEBUG
void CTChessGUIView::AssertValid() const
{
	CView::AssertValid();
}

void CTChessGUIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTChessGUIDoc* CTChessGUIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTChessGUIDoc)));
	return (CTChessGUIDoc*)m_pDocument;
}
#endif //_DEBUG


// CTChessGUIView message handlers
