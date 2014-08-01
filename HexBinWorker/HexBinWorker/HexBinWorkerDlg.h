
// HexBinWorkerDlg.h : ͷ�ļ�
//

#pragma once
#include "IntelHex.h"

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();


private:
	IntelHex _hexFile;
};
