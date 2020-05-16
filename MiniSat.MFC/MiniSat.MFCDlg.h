#pragma once


// CMiniSatMFCDlg ダイアログ
class CMiniSatMFCDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()
// コンストラクション
public:
	CMiniSatMFCDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINISATMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtCalculate();
	afx_msg void OnBnClickedBtOpenFile();
	afx_msg void OnBnClickedBtRead();
	afx_msg HCURSOR OnQueryDragIcon();
	
protected:
	CString m_inputString;
	CString m_outputString;
	CString m_inputFilePath;
};
