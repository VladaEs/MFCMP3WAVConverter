#pragma once
#include <afxdialogex.h>
#pragma once

#include "afxwin.h"
#include "ConvertedSample.h"
#include "mp3.hpp"
#include "Music.hpp"
#include "Helpers.hpp"
#include "Encoder.h"
#include <wmp.h>


// Диалоговое окно EditMusicDialog

class ConvertProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConvertProgressDlg)

public:

	// Стандартный конструктор
	ConvertProgressDlg(CWnd* pParent = nullptr);
	~ConvertProgressDlg()
	{
		KillTimer(1);
	}

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONVERTINGPROGRESS };
#endif

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP();

public:
	Music * music;


	INT_PTR DoModal() {
		CDialogEx::DoModal();

		int progress = 0;
		if (!this->music->loadMusic()) {
			return 0;
		}
		Encoder encoder;
		if (this->music->getExtension() == "wav") {
			auto* wav = dynamic_cast<WAV::WAVFile *>(this->music->file.get());
			if (wav) {
				encoder.Encode(*wav);
			}
		}
		
	}



	void ConvertProgressDlg::OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == 1) {
			Invalidate(FALSE);
		}
		CDialogEx::OnTimer(nIDEvent);
	}
	void ConvertProgressDlg::initMusic(Music *m) {
		this->music = m; 
		std::string ext = this->music->getExtension();
		Helpers helper;
		
	}

	BOOL OnInitDialog()
	{
		CDialogEx::OnInitDialog();
		Helpers helper;
		SetTimer(1, 16, nullptr);
		return TRUE;
	}
};





