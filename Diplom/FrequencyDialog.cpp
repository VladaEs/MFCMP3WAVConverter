#include "pch.h"
#include "FrequencyDialog.h"
#include "MusicPLayerDlg.h"
#include "Helpers.hpp"
#include "resource.h"
#include "EditMusicDialog.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(FrequencyDlg, CDialogEx)

FrequencyDlg::FrequencyDlg(CWnd* pParent)
	: CDialogEx(IDD_MUSIC_PLAYERDLG, pParent)
{
}



BEGIN_MESSAGE_MAP(FrequencyDlg, CDialogEx)

	ON_WM_PAINT()
	ON_WM_TIMER()

END_MESSAGE_MAP();
