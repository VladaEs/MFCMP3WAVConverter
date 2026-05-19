
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
	ON_COMMAND(ID_VIEW_OPEN, &CDiplomView::OnViewOpen)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
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

	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_VERT, &si);
	int scrollY = si.nPos;
	TRACE(_T("Scroll pos Y - %d"), scrollY);
	CPoint scrollPos(0, scrollY);
	musicController.drawPlayer(this, scrollPos);
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

std::string CDiplomView::ConvertToSTDString(CString str) {
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);
	return strStd;
}

CString CDiplomView::ConvertToCString(std::string str) {
	CString cstr(str.c_str());
	return cstr;
}
BOOL CDiplomView::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, *this->activeBG);
	return TRUE;
}

// CDiplomView message handlers
void CDiplomView::OnViewOpen()
{
	CFolderPickerDialog m_dlg;
	CString folderName;
	m_dlg.m_ofn.lpstrTitle = _T("Select a folder where your music files are");
	m_dlg.m_ofn.lpstrInitialDir = _T("C:\\");
	if (m_dlg.DoModal() == IDOK) {
		folderName = m_dlg.GetPathName();   // Use this to get the selected folder name 
										  // after the dialog has closed

		// May need to add a '\' for usage in GUI and for later file saving, 
		// as there is no '\' on the returned name
		folderName += _T("\\");
		UpdateData(FALSE);   // To show updated folder in GUI

		// Debug
		TRACE("\n%S", folderName);
	}
	this->MusicDirectory = folderName;
	fileSystemHandler.SetRootDir(ConvertToSTDString(MusicDirectory));
	this->activeBG = &bgColorMusicLoaded;
	//mp3Controller.SetRawData(this, fileSystemHandler.GetPathList());
	musicController.setRawData(this, fileSystemHandler.GetPathList());
	Invalidate();
	OnInitialUpdate();

}


void CDiplomView::OnLButtonDown(UINT nFlags, CPoint point) {
	for (int i = 0; i < musicController.getPlayButtons().size(); i++) {
		if (musicController.getPlayButtonDataById(i)->rect.PtInRect(point)) {
			TRACE(_T("PLAY BUTTON %d CLICKED\n"), i);
		}
	}
}