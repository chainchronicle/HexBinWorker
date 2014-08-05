
// HexBinWorkerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HexBinWorker.h"
#include "HexBinWorkerDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CHexBinWorkerDlg �Ի���
CHexBinWorkerDlg::CHexBinWorkerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHexBinWorkerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CHexBinWorkerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CHexBinWorkerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHexBinWorkerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE, &CHexBinWorkerDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_HEX_TO_BIN, &CHexBinWorkerDlg::OnBnClickedHexToBin)
	ON_BN_CLICKED(IDC_BIN_TO_HEX, &CHexBinWorkerDlg::OnBnClickedBinToHex)
END_MESSAGE_MAP()


// CHexBinWorkerDlg ��Ϣ�������

BOOL CHexBinWorkerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void CHexBinWorkerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHexBinWorkerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHexBinWorkerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHexBinWorkerDlg::showTextField() {
	// clear
	GetDlgItem(IDC_HEXFILEFIELD)->SetWindowText(_T(""));
	GetDlgItem(IDC_BINFILEFIELD)->SetWindowText(_T(""));

	CString hexText, binText;
	_hbController.getEditFieldText(hexText, binText);
	GetDlgItem(IDC_HEXFILEFIELD)->SetWindowText(hexText);
	GetDlgItem(IDC_BINFILEFIELD)->SetWindowText(binText);
}

void CHexBinWorkerDlg::showFilePath() {
	// clear
	GetDlgItem(IDC_HEX_PATH)->SetWindowText(_T(""));
	GetDlgItem(IDC_BIN_PATH)->SetWindowText(_T(""));

	CString hexPath, binPath;
	_hbController.getFilePath(hexPath, binPath);

	GetDlgItem(IDC_HEX_PATH)->SetWindowText(hexPath);
	GetDlgItem(IDC_BIN_PATH)->SetWindowText(binPath);
}


void CHexBinWorkerDlg::OnBnClickedOk()
{
	// Open
	CFileDialog pHexFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Inter Hex (*.hex)|*.hex|Bin File|*.bin||")); 
	CString filePathName; 

	if(pHexFileDlg.DoModal() == IDOK) {

		filePathName = pHexFileDlg.GetPathName(); 

		// Handle
		_hbController.init(filePathName);
		_hbController.parse(filePathName);

		// Show 
		showTextField();
		showFilePath();
	}
}


void CHexBinWorkerDlg::OnBnClickedHexToBin()
{
	// using hex file to write bin file
	_hbController.writeToBinFile();

	// read bin file
	_hbController.parseBin();

	showTextField();
}

void CHexBinWorkerDlg::OnBnClickedBinToHex()
{
	_hbController.writeToHexFile();
	_hbController.parseHex();

	//showFilePath();
	showTextField();
}

void CHexBinWorkerDlg::OnBnClickedSave()
{
	//_hexFile.writeToBinFile(); //TODO: add fileNAME
	//MessageBox(_T("saved"), NULL);

}






