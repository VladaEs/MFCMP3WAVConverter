#include "pch.h"
#include "FrequencyDialog.h"
#include "MusicPLayerDlg.h"
#include "Helpers.hpp"
#include "resource.h"
#include "ConvertProgressDlg.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(ConvertProgressDlg, CDialogEx)

ConvertProgressDlg::ConvertProgressDlg(CWnd* pParent)
	: CDialogEx(IDD_CONVERTINGPROGRESS, pParent)
{
}


BEGIN_MESSAGE_MAP(ConvertProgressDlg, CDialogEx)

	ON_WM_PAINT()
	ON_WM_TIMER()

END_MESSAGE_MAP();
