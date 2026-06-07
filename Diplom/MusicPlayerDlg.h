#pragma once

#include <afxwin.h>
#include <wmp.h>
#include "Music.hpp";
class MusicPLayerDlg : public CDialogEx
{

	DECLARE_DYNAMIC(MusicPLayerDlg)
public:
	Music* activeMusic = nullptr;
	CImage musicImage;
	CButton m_buttons[4]; // start, stop, pause, edit btns
	CSliderCtrl m_slider;



	IWMPPlayer* pPlayer = nullptr;
	IWMPControls* pControls = nullptr;
	IWMPSettings *pSettings = nullptr;

	HRESULT LoadImageFromMemory(const std::vector<char>& imageData, CImage& outImage);
	void MusicPLayerDlg::initMusic(Music *m);
	MusicPLayerDlg(CWnd* pParent = nullptr);
	virtual ~MusicPLayerDlg();


	bool setActiveMusic(Music &music) {
		this->activeMusic = &music;
		this->initMusic(this->activeMusic);
		//this->activeMusic.file
		return true;
	}
	Music* getActiveMusic() {
		return activeMusic;
	}

	afx_msg void OnPaint();
	afx_msg void OnBtnclickedPlay();
	afx_msg void OnBtnclickedPause();
	afx_msg void OnBtnclickedStop();
	afx_msg void OnBtnclickedEdit();
	afx_msg void OnBtnclickedConvert();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL OnInitDialog();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUSIC_PLAYERDLG};
#endif

protected:
	DECLARE_MESSAGE_MAP()
};