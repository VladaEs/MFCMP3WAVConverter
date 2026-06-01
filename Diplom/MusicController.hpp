#pragma once
#include <cstring>
#include <vector>
#include "Music.hpp"
#include <atlimage.h>
#include <filesystem>
#include "Music.hpp"
#include "Music.hpp"
#include "Helpers.hpp"
#include "pch.h"
namespace fs = std::filesystem;


struct PlayButtonData {
	CRect rect;
	int musicIndex;
};


class MusicController
{
	std::vector<Music> MusicCollection;
	int activeMusicIndex = 0;
	int _initX = 0;
	int _initY = 0;
	int _lastX = 0;
	int _lastY = 0;
	int paddingBT = 10;
	int paddingLR = 150;
	int blockHeight = 100;
	int blockWidth = 90; // percent
	int tableMargin = 20;

	std::vector<PlayButtonData> playButtons;
	Helpers helper;
public:
	MusicController() { ; }

	MusicController(std::vector<std::string> dataList) {
		for (const auto& fullPath : dataList) {
			std::string filename = fs::path(fullPath).stem().string();
			Music music(fullPath, filename);
			MusicCollection.push_back(std::move(music));

		}
	}
	MusicController& setRawData(CWnd* parent, std::vector<std::string> dataList) {
		int i = 0;
		for (const auto& fullPath : dataList) {
			std::string filename = fs::path(fullPath).stem().string();
			std::string extension = fs::path(fullPath).extension().string();
			Music music(parent, fullPath, filename, i);
			if (extension == ".mp3")
			{
				music.file = std::make_unique<MP3>();
				music.setExtension("mp3");
				TRACE(
					_T("AFTER SET: %S\n"),
					music.getExtension().c_str()
				);
			}
			else if (extension == ".wav")
			{
				music.file = std::make_unique<WAV::WAVFile>();
				music.setExtension("wav");
				TRACE(
					_T("AFTER SET: %S\n"),
					music.getExtension().c_str()
				);
			}
			else if (extension == ".muc") {
				music.file = std::make_unique<MUC::MUCFile>();
				music.setExtension("muc");
			}

			
			MusicCollection.push_back(std::move(music));
			TRACE(
				_T("VECTOR EXT: %S\n"),
				MusicCollection.back().getExtension().c_str()
			);
			++i;
		}
		return *this;
	}

	std::vector<Music>& getMusicCollection() {
		return this->MusicCollection;
	}
	std::vector<PlayButtonData> getPlayButtons() {
		return this->playButtons;
	}
	PlayButtonData *getPlayButtonDataById(int i) {
		if (i< 0 || i >= playButtons.size()) {
			
			return nullptr;
		}
		return &this->playButtons[i];
	}

	int GetBlockHeight() {
		return this->blockHeight;
	}
	void preparePageBackground() {

	}
	void drawPlayer(CWnd* parent, CPoint scrollPos)
	{
		if (!parent) return;
		playButtons.clear();
		CClientDC dc(parent);

		CRect clientRect;
		parent->GetClientRect(&clientRect);

		//dc.FillSolidRect(&clientRect, RGB(30, 30, 30));

		int rowHeight = 40;
		int startY = 50 - scrollPos.y;

		if (MusicCollection.size() >= 1) {
			drawTableHeader(&dc, clientRect);
		}
		else {
			drawMusicSelectHint(&dc, clientRect);
		}

		for (int i = 0; i < MusicCollection.size(); i++)
		{
			CRect rowRect(tableMargin, startY + (i * rowHeight), clientRect.Width() - tableMargin, startY + ((i + 1) * rowHeight));
			//TRACE(_T("startY = %d\n"), startY);
			//TRACE(_T("rowHeight = %d\n"), rowHeight);
			drawMusicRow(&dc, rowRect, i);
		}
	}
	void drawTableHeader(CClientDC* dc, CRect clientRect)
	{
		CFont font;
		font.CreatePointFont(120, _T("Segoe UI"));

		CFont* oldFont = dc->SelectObject(&font);

		int oldBkMode = dc->SetBkMode(TRANSPARENT);
		COLORREF oldTextColor = dc->SetTextColor(RGB(0, 0, 0));
		
		dc->TextOut(30, 15, _T("#"));
		dc->TextOut(80, 15, _T("Song Name"));
		dc->TextOut(clientRect.Width() - paddingLR * 3, 15, _T("Type"));
		dc->TextOut(clientRect.Width() - paddingLR *2, 15, _T("Duration"));
		dc->TextOut(clientRect.Width() - paddingLR, 15, _T("Play"));
		
		dc->MoveTo(20, 50);
		dc->LineTo(clientRect.Width() - 20, 50);

		dc->SetBkMode(oldBkMode);
		dc->SetTextColor(oldTextColor);

		dc->SelectObject(oldFont);
	}
	void drawMusicRow(CClientDC* dc, CRect rect, int i)
	{
		//dc->FillSolidRect(rect, RGB(0, 0, 0));
		//TRACE(_T("Rect: left=%d top=%d right=%d bottom=%d\n"), rect.left, rect.top, rect.right, rect.bottom);
		// LINE COLOR
		CPen pen(PS_SOLID, 1, RGB(80, 80, 80));

		CPen* oldPen = dc->SelectObject(&pen);

		dc->MoveTo(rect.left, rect.bottom);
		dc->LineTo(rect.right, rect.bottom);

		dc->SelectObject(oldPen);

		// FONT
		CFont font;
		font.CreatePointFont(110, _T("Segoe UI"));

		CFont* oldFont = dc->SelectObject(&font);

		int oldBkMode = dc->SetBkMode(TRANSPARENT);

		//COLORREF oldTextColor = dc->SetTextColor(RGB(230, 230, 230));

		COLORREF oldTextColor = dc->SetTextColor(RGB(0, 0, 0));

		CString index;
		index.Format(_T("%d"), i + 1);

		dc->TextOut(rect.left + 10, rect.top + 10, index);

		dc->TextOut(
			rect.left + 60,
			rect.top + 10,
			helper.ConvertToCString(
				MusicCollection[i].GetMusicName()
			)
		);


		CRect playButton(rect.right - paddingLR + tableMargin, rect.top + 10, rect.right - 20, rect.top + 35);
		PlayButtonData buttonData;

		buttonData.musicIndex = i;
		buttonData.rect = playButton;

		playButtons.push_back(buttonData);
		dc->FillSolidRect(playButton, RGB(0, 120, 215));

		dc->TextOut(
			(rect.right - paddingLR * 3) + tableMargin,
			rect.top + 10,
			helper.ConvertToCString(
				MusicCollection[i].getExtension()
			)
		);

		dc->TextOut(
			(rect.right - paddingLR * 2) + tableMargin,
			rect.top + 10,
			_T("03:45")
		);


		dc->DrawText(
			_T("Play"),
			playButton,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);

		dc->SetBkMode(oldBkMode);
		dc->SetTextColor(oldTextColor);

		dc->SelectObject(oldFont);
	}

	void drawMusicSelectHint(CClientDC* dc, CRect clientRect) {
		CFont font;
		int centerX = clientRect.Width() / 2;
		int centerY = clientRect.Height() / 2;
		int offset = 0;
		font.CreatePointFont(150, _T("Segoe UI"));
		CFont * oldFont = dc->SelectObject(&font);
		
		CString projectName = _T("Music Player and Wave Compressor");
		CSize projectNameSize = dc->GetTextExtent(projectName);
		dc->TextOut(centerX - projectNameSize.cx / 2, (centerY - projectNameSize.cy / 2) + offset, projectName);
		offset = offset + projectNameSize.cy + 5;

		CFont smallFont;
		smallFont.CreatePointFont(110, _T("Segoe UI"));
		dc->SelectObject(&smallFont);
		CString author = _T("Made By Voronin Vlad");
		CSize authorSize = dc->GetTextExtent(author);
		dc->TextOut(centerX - authorSize.cx / 2, (centerY - authorSize.cy / 2) + offset, author);
		offset = offset + authorSize.cy + 5;

		CString manualHint = _T("Please Choose A folder in the menu");
		CSize manualHintSize = dc->GetTextExtent(manualHint);
		dc->TextOut(centerX - manualHintSize.cx/2,( centerY - manualHintSize.cy / 2) + offset, manualHint);
		dc->SelectObject(oldFont);
		offset = offset + manualHintSize.cy + 5;
	};

};

