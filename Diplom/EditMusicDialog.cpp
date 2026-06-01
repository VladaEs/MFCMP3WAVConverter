// EditMusicDialog.cpp: файл реализации
//

#include "pch.h"
#include "diplom.h"
#include "EditMusicDialog.h"
#include "afxdialogex.h"
#include <string>
#include "mp3.hpp"
#include "Music.hpp"
#include "MusicController.hpp"
#include <shlwapi.h>
#include "resource.h"
#include <wmp.h>



IMPLEMENT_DYNAMIC(EditMusicDialog, CDialogEx)


BEGIN_MESSAGE_MAP(EditMusicDialog, CDialogEx)
	ON_WM_PAINT()


	ON_EN_CHANGE(IDC_MUSICNAME, &EditMusicDialog::OnEnChangeMusicname)
	ON_EN_CHANGE(IDC_MUSICAUTHOR, &EditMusicDialog::OnEnChangeMusicauthor)
	ON_BN_CLICKED(IDC_CHANGEIMAGE, &EditMusicDialog::OnBnClickedChangeimage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

EditMusicDialog::EditMusicDialog(CWnd* pParent)
	: CDialogEx(IDD_MUSICEDITDLG, pParent)  // Инициализируем указатель как nullptr
{
}

// Конструктор с параметром Music

void EditMusicDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}
void EditMusicDialog::OnClose() {
	//AfxMessageBox(L"Here");
	//if (newImage.size() == 0)
	TRACE("MusicName: %s", this->musicNameInput);
	TRACE("MusicAuthor: %s", this->authorNameInput);
	if (ConvertCStringToVectorChar(this->musicNameInput).size() > 0) {
		m_music->setTag("TIT2", ConvertCStringToVectorChar(this->musicNameInput));
	}
	if (ConvertCStringToVectorChar(this->authorNameInput).size() > 0) {
		m_music->setTag("TPE1", ConvertCStringToVectorChar(this->authorNameInput));
	}
	if (this->newImage.size() > 0) {
		m_music->setTag("APIC", this->newImage);
	}
	m_music->saveFile();
	CDialogEx::OnClose();
}


void EditMusicDialog::OnOK()
{
    GetDlgItemText(
        IDC_MUSICNAME,
        musicNameInput
    );

    GetDlgItemText(
        IDC_MUSICAUTHOR,
        authorNameInput
    );

    TRACE(_T("MusicName: %s\n"), musicNameInput);
    TRACE(_T("MusicAuthor: %s\n"), authorNameInput);

    if (!musicNameInput.IsEmpty())
    {
        m_music->setTag(
            "TIT2",
            ConvertCStringToVectorChar(
                musicNameInput
            )
        );
    }

    if (!authorNameInput.IsEmpty())
    {
        m_music->setTag(
            "TPE1",
            ConvertCStringToVectorChar(
                authorNameInput
            )
        );
    }

    if (!newImage.empty())
    {
        m_music->setTag(
            "APIC",
            newImage
        );
    }

    m_music->saveFile();

    CDialogEx::OnOK();
}


EditMusicDialog::~EditMusicDialog() {


}




BOOL EditMusicDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	std::string MusicNameCleaned = this->cleanTag(m_music->getTag("TIT2"));
	std::string text = "MP3 loaded: " + this->cleanTag(m_music->getTag("TIT2"));

	CString ctext(text.c_str());

	AfxMessageBox(ctext);
	CString MusicName = ConvertToCString(MusicNameCleaned);
	SetDlgItemText(IDC_MUSICNAME, MusicName);

	std::string authorString = this->cleanTag(m_music->getTag("TPE1"));
	CString author = ConvertToCString(authorString);
	SetDlgItemText(IDC_MUSICAUTHOR, author);
	text = "MP3 loaded: " + this->cleanTag(m_music->getTag("TPE1"));

	ctext = (text.c_str());

	AfxMessageBox(ctext);
	this->musicNameInput = MusicName;
	this->authorNameInput = author;
	return TRUE;
}
void EditMusicDialog::initMusic(Music *m) {
	this->m_music = m;
	m_music->loadMusic();
	std::string pathWay = m_music->GetPath();
}



void EditMusicDialog::OnPaint() {

}




CString EditMusicDialog::ConvertToCString(std::string str) {
	CString cstr(str.c_str());
	return cstr;
}
std::string EditMusicDialog::cleanTag(std::vector<char>& data) {
	std::string res;
	for (int i = 0; i < data.size(); i++) {
		if (data[i] >= 32 && data[i] <= 128) {
			res += data[i];
		}
	}
	return res;
}
std::string EditMusicDialog::ConvertCharVectToString(std::vector<char> &data) {

	std::string res(data.size(), ' ');
	for (int i = 0; i < data.size(); i++) {
		res[i] = data[i];
	}
	return res;
}
std::string EditMusicDialog::ConvertCStringToString(CString str) {
	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);
	return strStd;
}
std::vector<char> EditMusicDialog::ConvertCStringToVectorChar(CString str) {
	const wchar_t* wstr = str.GetString();
	int len = str.GetLength();


	std::vector<char> vec(reinterpret_cast<const char*>(wstr),
		reinterpret_cast<const char*>(wstr + len));
	return vec;
}

void EditMusicDialog::OnEnChangeMusicname() {
	GetDlgItemText(IDC_MUSICNAME, this->musicNameInput);
}


void EditMusicDialog::OnEnChangeMusicauthor() {
	GetDlgItemText(IDC_MUSICAUTHOR, this->authorNameInput);
}


void EditMusicDialog::OnBnClickedChangeimage() {
	CString filePath;

	CFileDialog dialog(TRUE, _T(".jpg"), NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Image Files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp|All Files (*.*)|*.*||"),
		this);
	if (dialog.DoModal() == IDOK) {
		filePath = dialog.GetPathName();
		CT2CA pszConvertedAnsiString(filePath);
		std::string strStd(pszConvertedAnsiString);
		this->newImage = this->ReadFileAsBytes(strStd);
	}
}


std::vector<char> EditMusicDialog::ReadFileAsBytes(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	std::vector<char> data((std::istreambuf_iterator<char>(file)), {});


	//data.push_back('\0');
	return data;

}