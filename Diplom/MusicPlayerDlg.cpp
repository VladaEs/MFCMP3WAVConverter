
#include "pch.h"
#include "MusicPLayerDlg.h"
#include "Helpers.hpp"
#include "resource.h"

#include <algorithm>

IMPLEMENT_DYNAMIC(MusicPLayerDlg, CDialogEx)

MusicPLayerDlg::MusicPLayerDlg(CWnd* pParent)
	: CDialogEx(IDD_MUSIC_PLAYERDLG, pParent)
{
}



BEGIN_MESSAGE_MAP(MusicPLayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(3001, &MusicPLayerDlg::OnBtnclickedPlay)
	ON_BN_CLICKED(3002, &MusicPLayerDlg::OnBtnclickedPause)
	ON_BN_CLICKED(3003, &MusicPLayerDlg::OnBtnclickedEdit)
	//ON_BN_CLICKED(3004, &DrawMusicDialog::OnBtnclickedStop)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


MusicPLayerDlg::~MusicPLayerDlg() {

	if (pControls)
	{
		pControls->pause();
		pControls->Release();
		pControls = nullptr;
	}
	if (pPlayer)
	{
		pPlayer->close();
		pPlayer->Release();
		pPlayer = nullptr;
	}
	if (pSettings) {

		pSettings->Release();
		pSettings = nullptr;
	}

}



BOOL MusicPLayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	Helpers helper;
	int buttonsStartX = 20;
	int buttonsStartY = 350;
	int buttonWidth = 100;
	int buttonHeight = 50;
	int margin = 20;
	std::vector<std::string> buttonNames = { "Play", "Pause" , "Edit" };

	int lastX = 0;
	for (int i = 0; i < buttonNames.size(); i++) {
		int x = buttonsStartX + i * (buttonWidth + margin);
		int y = buttonsStartY;
		CRect rect(x, y, x + buttonWidth, y + buttonHeight);
		m_buttons[i].Create(helper.ConvertToCString(buttonNames[i]), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, this, 3001 + i);
	}

	CRect rect;
	GetClientRect(&rect);
	int y = buttonsStartY + buttonHeight + margin;
	CRect sliderRect(25, buttonsStartY + buttonHeight + margin, rect.Width() - 25, buttonsStartY + buttonHeight + margin * 3);
	m_slider.Create(
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ, // стиль
		sliderRect,
		this,
		4001);

	m_slider.SetRange(0, 100);    // от 0 до 100
	m_slider.SetPos(0);           // начальная позиция
	m_slider.SetTicFreq(1);      // шаг между делениями


	// init music controlls : 

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
		hr = CoCreateInstance(__uuidof(WindowsMediaPlayer), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pPlayer));
		if (SUCCEEDED(hr)) {
			CString musicPath = helper.ConvertToCString(activeMusic.GetPath());
			BSTR bstrFile = SysAllocString(musicPath);
			pPlayer->put_URL(bstrFile);
			SysFreeString(bstrFile);
			hr = pPlayer->get_controls(&pControls);
			pPlayer->get_settings(&pSettings);
			//pSettings->put_repeat(VARIANT_TRUE);
		}
	}


	// get music Lenght 



	// init timer

	SetTimer(5001, 1000, NULL);





	return TRUE;
}




void MusicPLayerDlg::OnPaint()
{

	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect, RGB(255, 255, 255));

	//CString text = L"AAA";

	//dc.DrawText(text, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// draw BG



	// -=-=-=-=-=-=-=-=-=-=-=-=-=-= draw Image -=-=-=-=-=-=-=-=-=-=-=-=-=-=
	if (!musicImage.IsNull()) {
		int imageWidth = 200;
		int imageHeight = 200;
		//int yImageStart = (int)(rect.Height() / 2) - (int)(imageHeight/2);
		int xImageStart = (int)(rect.Width() / 2) - (int)(imageWidth / 2);
		int yImageStart = 25;
		CRect imageRect(xImageStart, yImageStart, xImageStart + imageWidth, yImageStart + imageHeight);
		musicImage.Draw(dc.m_hDC, imageRect);
	}
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-= Finish draw Image -=-=-=-=-=-=-=-=-=-=-=-=-=-=



	Helpers help;

	CFont font;
	font.CreatePointFont(120, _T("Segoe UI"));
	CFont* pOldFont = dc.SelectObject(&font);
	std::string MusicNameCleaned = help.cleanTag(activeMusic.getTag("TIT2"));
	CString MusicName = help.ConvertToCString(MusicNameCleaned);

	CRect imageRect2(25, 250, rect.Width() - 25, 270);
	dc.DrawText(MusicName, &imageRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	CRect imageRect3(25, 280, rect.Width() - 25, 300);
	std::string authorString = help.cleanTag(activeMusic.getTag("TPE1"));
	CString author = help.ConvertToCString(authorString);
	dc.DrawText(author, &imageRect3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pOldFont);
}

HRESULT MusicPLayerDlg::LoadImageFromMemory(const std::vector<char>& rawData, CImage& outImage)
{
	if (rawData.empty())
		return E_INVALIDARG;

	// --- Поиск сигнатуры изображения ---
	const std::vector<std::vector<char>> signatures = {
		{ '\xFF', '\xD8' },                   // JPEG
		{ '\x89', 'P',  'N',  'G' },          // PNG
		{ 'B',   'M' },                       // BMP
		{ 'G',   'I',  'F' }                  // GIF
	};

	auto begin = rawData.begin();
	auto end = rawData.end();
	auto found = end;

	for (const auto& sig : signatures)
	{
		auto it = std::search(begin, end, sig.begin(), sig.end());
		if (it != end && (found == end || it < found))
		{
			found = it; // находим самое раннее валидное начало
		}
	}

	if (found == end) {
		// AfxMessageBox(L"Формат изображения не распознан");
		return E_FAIL;
	}

	std::vector<char> cleanData(found, end);

	// --- Создаём HGLOBAL ---
	HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, cleanData.size());
	if (!hMem)
		return E_OUTOFMEMORY;

	void* pMem = ::GlobalLock(hMem);
	if (!pMem) {
		::GlobalFree(hMem);
		return E_FAIL;
	}

	memcpy(pMem, cleanData.data(), cleanData.size());
	::GlobalUnlock(hMem);

	// --- Создаём IStream ---
	CComPtr<IStream> spStream;
	HRESULT hr = ::CreateStreamOnHGlobal(hMem, TRUE, &spStream);
	if (FAILED(hr)) {
		::GlobalFree(hMem);
		return hr;
	}

	// --- Загружаем изображение ---
	hr = outImage.Load(spStream);
	if (FAILED(hr) || outImage.IsNull()) {
		// AfxMessageBox(L"Не удалось загрузить изображение");
		return E_FAIL;
	}

	return S_OK;
}


void MusicPLayerDlg::OnClose() {
}
void MusicPLayerDlg::OnBtnclickedPlay()
{
	WMPPlayState state;
	pPlayer->get_playState(&state);
	if (state == wmppsPlaying) {
		pControls->pause();
	}
	else if (state == wmppsPaused || state == wmppsStopped) {
		pControls->play();
	}

	//pControls->play();
	//pControls->Release();
}

void MusicPLayerDlg::OnBtnclickedPause()
{
	WMPPlayState state;
	pPlayer->get_playState(&state);
	if (state == wmppsPlaying)
	{
		pControls->pause();
	}
	else if (state == wmppsPaused || state == wmppsStopped)
	{
		pControls->play();
	}
	AfxMessageBox(L"Pause clicked");

}

void MusicPLayerDlg::OnBtnclickedStop()
{

	AfxMessageBox(L"Stop clicked");

}


void MusicPLayerDlg::OnBtnclickedEdit()
{
	/*
	EditMusicDialog dlg;
	dlg.initMusic(activeMusic);
	pPlayer->close();
	dlg.DoModal();
	*/
}


void MusicPLayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {

	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == m_slider.GetSafeHwnd())
	{
		int sliderPos = m_slider.GetPos();
		int sliderMax = m_slider.GetRangeMax();

		double sliderPosPerc = (sliderPos * 100) / sliderMax;
		double exactSecond = (sliderPosPerc * activeMusic.getMusicDuration()) / 100;

		pControls->put_currentPosition(exactSecond);
		//CString msg;
		//msg.Format(L"Позиция слайдера: %d", sliderPosPerc);

		//AfxMessageBox((msg));

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

}


void MusicPLayerDlg::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == 5001) {
		IWMPMedia* pMedia = nullptr;
		double duration = 0.0;

		HRESULT hr2 = pPlayer->get_currentMedia(&pMedia);

		if (SUCCEEDED(hr2) && pMedia) {
			pMedia->get_duration(&duration);
			activeMusic.setMusicDuration(duration);
			pMedia->Release();
		}
		WMPPlayState state;
		pPlayer->get_playState(&state);

		if (state == wmppsPlaying) {

			int sliderMin = 0;
			int sliderMax = m_slider.GetRangeMax();

			double musicSeconds = 0;
			pControls->get_currentPosition(&musicSeconds);
			activeMusic.SetCurrentMusicTime(musicSeconds);
			int sliderPos = 0;
			if (activeMusic.getMusicDuration() > 0.0) {
				sliderPos = (int)((musicSeconds / activeMusic.getMusicDuration()) * sliderMax);
			}
			m_slider.SetPos(sliderPos);

		}

	}
}