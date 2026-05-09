
// DiplomView.cpp : implementation of the CDiplomView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Diplom.h"
#endif

#include "DiplomDoc.h"
#include "DiplomView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDiplomView

IMPLEMENT_DYNCREATE(CDiplomView, CView)

BEGIN_MESSAGE_MAP(CDiplomView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDiplomView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CDiplomView construction/destruction

CDiplomView::CDiplomView() noexcept
{
	// TODO: add construction code here

}

CDiplomView::~CDiplomView()
{
}

BOOL CDiplomView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDiplomView drawing

void CDiplomView::OnDraw(CDC* /*pDC*/)
{
	CDiplomDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CDiplomView printing


void CDiplomView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CDiplomView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDiplomView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDiplomView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CDiplomView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDiplomView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDiplomView diagnostics

#ifdef _DEBUG
void CDiplomView::AssertValid() const
{
	CView::AssertValid();
}

void CDiplomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDiplomDoc* CDiplomView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDiplomDoc)));
	return (CDiplomDoc*)m_pDocument;
}
#endif //_DEBUG


// CDiplomView message handlers
