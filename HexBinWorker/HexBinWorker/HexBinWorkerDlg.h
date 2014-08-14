
// HexBinWorkerDlg.h : ͷ�ļ�
#pragma once
#include "CnComm.h"
#include "HexBinController.h"
#include "HexEdit.h"


// CHexBinWorkerDlg �Ի���
class CHexBinWorkerDlg : public CDialogEx
{
// ����
public:
	CHexBinWorkerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HEXBINWORKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedHexToBin();
	afx_msg void OnBnClickedBinToHex();
	afx_msg void OnBnClickedBtnOpencom();
	DECLARE_MESSAGE_MAP()

public:

	void showTextField();
	void showFilePath();
	void getTextField(string& hexText, string& binText);

	// redo Edit Text 
	
	void showHexEditText();
	void getHexEditText(string& hexText);

	void showBinEditText();
	void getBinEditText(BYTE *pDatas, int dataSize);



	// to be rearraged..
	CnComm _hCom;
	void findAvailableCom();
	void showAvailableCom(const vector<CString> &aCom);
	

private:
	// -UI
	CFont _editFont;

	// -Controller
	HexBinController _hbController;
	CHexEdit _hexEdit;

	
};
