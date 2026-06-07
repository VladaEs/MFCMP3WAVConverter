#pragma once
#include <afxdialogex.h>
#pragma once

#include "afxwin.h"
#include "ConvertedSample.h"
#include "mp3.hpp"
#include "Music.hpp"
#include "Helpers.hpp"
#include <wmp.h>


// Диалоговое окно EditMusicDialog

class FrequencyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FrequencyDlg)

public:

	// Стандартный конструктор
	FrequencyDlg(CWnd* pParent = nullptr);
	~FrequencyDlg()
	{
		KillTimer(1);
	}

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FREQUENCY };
#endif

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP();

public:
	std::vector<WAV::ConvertedSample> spectrumData;




	void FrequencyDlg::OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == 1){
			Invalidate(FALSE);
		}
		CDialogEx::OnTimer(nIDEvent);
	}


	BOOL OnInitDialog()
	{
		CDialogEx::OnInitDialog();
		Helpers helper;
		SetTimer(1, 16, nullptr);
		return TRUE;
	}
};





