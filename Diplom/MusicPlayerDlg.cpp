
#include "pch.h"
#include "MusicPLayerDlg.h"
#include "Helpers.hpp"
#include "resource.h"
#include "EditMusicDialog.h"
#include "ConvertProgressDlg.h"
#include "FrequencyDialog.h"
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
	ON_BN_CLICKED(3004, &MusicPLayerDlg::OnBtnclickedConvert)
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
	this->activeMusic->loadMusic();
	int buttonsStartX = 20;
	int buttonsStartY = 350;
	int buttonWidth = 100;
	int buttonHeight = 50;
	int margin = 20;
	std::map<int, std::string> buttonNames =
{
    {1, "Play"},
    {2, "Pause"},

};
	if (this->activeMusic->extension == "mp3") {
		buttonNames[3] = "Edit";
	}
	if (this->activeMusic->extension == "wav") {
		buttonNames[4] = "Convert";
	}

	int lastX = 0; 
	int i = 0;
	for (const auto&[id, name] : buttonNames){
		int x = buttonsStartX + i * (buttonWidth + margin);
		int y = buttonsStartY;
		CRect rect(
			x,
			y,
			x + buttonWidth,
			y + buttonHeight);
		m_buttons[i].Create( helper.ConvertToCString(name), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, this, 3000 + id); // можно использовать id
		++i;
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
			CString musicPath = helper.ConvertToCString(activeMusic->getPlayableURL());
			BSTR bstrFile = SysAllocString(musicPath);
			pPlayer->put_URL(bstrFile);
			SysFreeString(bstrFile);
			hr = pPlayer->get_controls(&pControls);
			pPlayer->get_settings(&pSettings);
			//pSettings->put_repeat(VARIANT_TRUE);
		}
	}

	// init timer

	SetTimer(5001, 1000, NULL);
	SetTimer( 100, 33, NULL);




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


	auto& apic = activeMusic->getTag("APIC");


	// -=-=-=-=-=-=-=-=-=-=-=-=-=-= draw Image -=-=-=-=-=-=-=-=-=-=-=-=-=-=
	if (!musicImage.IsNull() && activeMusic->getExtension() != "wav") {
		int imageWidth = 200;
		int imageHeight = 200;
		//int yImageStart = (int)(rect.Height() / 2) - (int)(imageHeight/2);
		int xImageStart = (int)(rect.Width() / 2) - (int)(imageWidth / 2);
		int yImageStart = 25;
		CRect imageRect(xImageStart, yImageStart, xImageStart + imageWidth, yImageStart + imageHeight);
		musicImage.Draw(dc.m_hDC, imageRect);
	}
	else {
		spectrumRect = CRect( 25, 100, rect.Width() - 25, 270);
		DrawSpectrum( dc, spectrumRect);
	}
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-= Finish draw Image -=-=-=-=-=-=-=-=-=-=-=-=-=-=




	Helpers help;

	CFont font;
	font.CreatePointFont(120, _T("Segoe UI"));
	CFont* pOldFont = dc.SelectObject(&font);
	std::string MusicNameCleaned = help.cleanTag(activeMusic->getTag("TIT2"));
	CString MusicName = help.ConvertToCString(MusicNameCleaned);

	CRect imageRect2(25, 250, rect.Width() - 25, 270);
	dc.DrawText(MusicName, &imageRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	CRect imageRect3(25, 280, rect.Width() - 25, 300);
	std::string authorString = help.cleanTag(activeMusic->getTag("TPE1"));
	CString author = help.ConvertToCString(authorString);
	dc.DrawText(author, &imageRect3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pOldFont);
}

void MusicPLayerDlg::initMusic(Music *m) {
	this->activeMusic = m;
	activeMusic->loadMusic();
	std::string pathWay = activeMusic->GetPath();
	LoadImageFromMemory(activeMusic->getTag("APIC"), this->musicImage); // creating CImage from std::vector<char> array
	currentSpectrum.resize(64);

	for (int i = 0; i < 64; i++)
	{
		currentSpectrum[i] =
			(float)(rand() % 100);
	}
}

HRESULT MusicPLayerDlg::LoadImageFromMemory(const std::vector<char>& rawData, CImage& outImage)
{
	if (rawData.empty()) {
		TRACE("Showing Default one\n");
		CString dir(
			std::filesystem::current_path().wstring().c_str());

		TRACE(_T("Current dir: %s\n"), dir.GetString());
		outImage.Load(L"Debug/images/default.jpg");
		return S_OK;
	}
	if (!outImage.IsNull())
	{
		outImage.Destroy();
	}
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
		 //AfxMessageBox(L"Формат изображения не распознан");
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
	//TRACE("CImage::Load hr = 0x%08X\n", hr);
	if (FAILED(hr) || outImage.IsNull()) {
		TRACE("\nFAILED TO LOAD IMAGE\n");
		outImage.Load(L"images/default.jpg");
		// AfxMessageBox(L"Не удалось загрузить изображение");
		return E_FAIL;
	}

	return S_OK;
}

void MusicPLayerDlg::DrawSpectrum( CDC& dc, const CRect& rect)
{
	if (currentSpectrum.empty())
		return;

	const int bars = (int)128;
	const int usefulBins = 256;

	int width = rect.Width();

	int height = rect.Height()*1.3;

	int samplesPerBar = currentSpectrum.size() / bars;
	
	float maxValue = 1.0f;
	for (float v : currentSpectrum)
	{
		float a = fabsf(v);
		if (a > maxValue)
			maxValue = a;
	}
	int barWidth = max(1, width / bars);
	TRACE(
		"bars=%d width=%d height=%d barWidth=%d max=%f\n",
		bars,
		width,
		height,
		barWidth,
		maxValue);
	for (int i = 0; i < bars; i++)
	{
		float value = 0.0f;

		for (int j = 0; j < samplesPerBar; j++)
		{
			value += fabsf(currentSpectrum[i * samplesPerBar + j]);
		}

		value /= samplesPerBar;

		float normalized = value / maxValue;

		int barHeight = (int)(normalized * height);
		int x = rect.left + i * barWidth;
		int y = rect.bottom - barHeight;
		if (i < 5)
		{
			TRACE(
				"i=%d value=%f norm=%f h=%d\n",
				i,
				value,
				normalized,
				barHeight);
		}
		dc.Rectangle( x, y, x + 4, rect.bottom);
	}
}


void MusicPLayerDlg::OnClose() {
}
void MusicPLayerDlg::OnBtnclickedPlay()
{
	if (activeMusic->getExtension() != ".muc") {
		WMPPlayState state;
		pPlayer->get_playState(&state);
		if (state == wmppsPlaying) {
			pControls->pause();
		}
		else if (state == wmppsPaused || state == wmppsStopped) {
			pControls->play();
		}
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

}

void MusicPLayerDlg::OnBtnclickedStop()
{

	//AfxMessageBox(L"Stop clicked");

}


void MusicPLayerDlg::OnBtnclickedEdit()
{
	
	EditMusicDialog dlg;
	dlg.initMusic(activeMusic);
	pPlayer->close();
	dlg.DoModal();
	
}


void MusicPLayerDlg::OnBtnclickedConvert()
{
	ConvertProgressDlg dlg;
	dlg.initMusic(activeMusic);
	dlg.DoModal();

	IDD_CONVERTINGPROGRESS;

}



void MusicPLayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {

	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == m_slider.GetSafeHwnd())
	{
		int sliderPos = m_slider.GetPos();
		int sliderMax = m_slider.GetRangeMax();

		double sliderPosPerc = (sliderPos * 100) / sliderMax;
		double exactSecond = (sliderPosPerc * activeMusic->getMusicDuration()) / 100;

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
			activeMusic->setMusicDuration(duration);
			pMedia->Release();
		}
		WMPPlayState state;
		pPlayer->get_playState(&state);

		if (state == wmppsPlaying) {

			int sliderMin = 0;
			int sliderMax = m_slider.GetRangeMax();

			double musicSeconds = 0;
			pControls->get_currentPosition(&musicSeconds);
			activeMusic->SetCurrentMusicTime(musicSeconds);
			int sliderPos = 0;
			if (activeMusic->getMusicDuration() > 0.0) {
				sliderPos = (int)((musicSeconds / activeMusic->getMusicDuration()) * sliderMax);
			}
			m_slider.SetPos(sliderPos);

		}

	}

	if (nIDEvent == 100)
	{

		double musicSeconds = 0;

		pControls->get_currentPosition(
			&musicSeconds);

		auto window = activeMusic->file->getCustomWindow(musicSeconds);
		/*
		TRACE("Window size = %zu\n", window.size());*/
		encoder.MDCT( window, currentSpectrum);
		float mn = FLT_MAX;
		float mx = -FLT_MAX;

		for (float v : currentSpectrum)
		{
			if (v < mn) mn = v;
			if (v > mx) mx = v;
		}
		/*
		TRACE(
			"Spectrum range: %f .. %f\n", mn, mx);*/
		for (float& v : currentSpectrum)
		{
			v = log10f( fabsf(v) + 1.0f);
		}
		InvalidateRect(&spectrumRect, FALSE);
	}

}