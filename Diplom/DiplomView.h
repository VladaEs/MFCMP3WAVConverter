
// DiplomView.h : interface of the CDiplomView class
//
#include <vector>
#include "FileSystemController.h"
#include "MusicController.hpp"
#pragma once


class CDiplomView : public CView
{
protected: // create from serialization only
	CDiplomView() noexcept;
	DECLARE_DYNCREATE(CDiplomView)

// Attributes
public:
	CDiplomDoc* GetDocument() const;
// Operations
public:
	CString MusicDirectory;
	FSC::FileSystemController fileSystemHandler;
// Overrides
public:

	std::string ConvertToSTDString(CString str);
	CString ConvertToCString(std::string str);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	COLORREF bgColorInitial = RGB(255,255,255);
	COLORREF bgColorMusicLoaded = RGB(255, 239, 255);
	COLORREF *activeBG = &bgColorInitial;

	MusicController musicController;


protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CDiplomView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewOpen();
};


#ifndef _DEBUG  // debug version in DiplomView.cpp
inline CDiplomDoc* CDiplomView::GetDocument() const
   { return reinterpret_cast<CDiplomDoc*>(m_pDocument); }
#endif

