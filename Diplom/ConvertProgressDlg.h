#pragma once
#include <afxdialogex.h>


#include "afxwin.h"
#include "ConvertedSample.h"
#include "mp3.hpp"
#include "Music.hpp"
#include "Helpers.hpp"
#include "Encoder.h"
#include <filesystem>
#include <wmp.h>


// Диалоговое окно EditMusicDialog

class ConvertProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConvertProgressDlg)

public:

	int progress = 0; // max 100
	const int progressMax = 100;

	// Стандартный конструктор
	ConvertProgressDlg(CWnd* pParent = nullptr);
	/*
	~ConvertProgressDlg()
	{
		KillTimer(1);
	}
	*/

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONVERTINGPROGRESS };
#endif

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP();
	//afx_msg void OnPaint();
public:
	Music * music;



	afx_msg void OnPaint()
	{
		CPaintDC dc(this);

		CRect rect;
		GetClientRect(&rect);

		// фон
		dc.FillSolidRect(rect, RGB(255, 255, 255));

		// рамка прогресс бара
		CRect barRect(20, 40, rect.Width() - 20, 70);
		dc.Rectangle(barRect);

		// заполненная часть
		int width =
			(barRect.Width() * progress) / 100;

		CRect fillRect(
			barRect.left,
			barRect.top,
			barRect.left + width,
			barRect.bottom);

		dc.FillSolidRect(fillRect, RGB(0, 120, 215));

		// текст процента
		CString txt;
		txt.Format(L"%d%%", progress);

		dc.TextOutW(20, 10, txt);

		
	}
	
	void OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == 1) {
			Invalidate(FALSE);
		}
		CDialogEx::OnTimer(nIDEvent);
	}
	void initMusic(Music *m) {
		this->music = m; 
		std::string ext = this->music->getExtension();
		Helpers helper;
		
	}



	BOOL OnInitDialog()
	{
		CDialogEx::OnInitDialog();
		Helpers helper;
		SetTimer(1, 16, nullptr);
		AfxBeginThread( ConvertThread, this);


		return TRUE;
	}

	static UINT ConvertThread(LPVOID pParam) {
		auto* dlg = static_cast<ConvertProgressDlg*>(pParam);
		dlg->progress = 0;
		if (!dlg->music->loadMusic()) return 0;
		Encoder encoder;
		dlg->progress = 30;

		MUC::MUCFile convertedFile;

		auto* wav =
			dynamic_cast<WAV::WAVFile*>(
				dlg->music->file.get());

		if (wav)
		{
			encoder.setProgressCallback([dlg](int percent)
			{
				dlg->progress = percent;
			});
			convertedFile = encoder.Encode(*wav);
		}

		dlg->progress = 80;

		std::filesystem::path p(
			dlg->music->GetPath());

		std::string filePath =
			p.parent_path().string() +
			"/" +
			dlg->music->GetMusicName() +
			convertedFile.getExtension();

		convertedFile.write(filePath.c_str());

		dlg->progress = 100;
		return 0; 

	}

};





