
// MiniSat.MFCDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MiniSat.MFC.h"
#include "MiniSat.MFCDlg.h"
#include "afxdialogex.h"

#include <errno.h>
#include <zlib.h>
#include <string>


#undef max; // Required for "Vec.h".
#include "../minisat/utils/System.h"
#include "../minisat/utils/ParseUtils.h"
#include "../minisat/utils/Options.h"
#include "../minisat/core/Dimacs.h"
#include "../minisat/core/Solver.h"
#include "../minisat/core/MainFunc.h"



#include "../minisat/core/MainFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMiniSatMFCDlg ダイアログ



CMiniSatMFCDlg::CMiniSatMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MINISATMFC_DIALOG, pParent)
    , m_inputString(_T(""))
    , m_outputString(_T(""))
    , m_inputFilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiniSatMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    const int chars_limit = 99999999;
    DDX_Text(pDX, IDC_EDIT_INPUT, m_inputString);
    DDV_MaxChars(pDX, m_inputString, chars_limit);
    DDX_Text(pDX, IDC_EDIT_OUTPUT, m_outputString);
    DDV_MaxChars(pDX, m_outputString, chars_limit);
    DDX_Text(pDX, IDC_EDIT1, m_inputFilePath);
}

BEGIN_MESSAGE_MAP(CMiniSatMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_CALCULATE, &CMiniSatMFCDlg::OnBnClickedBtCalculate)
    ON_BN_CLICKED(IDC_BT_OPEN_FILE, &CMiniSatMFCDlg::OnBnClickedBtOpenFile)
    ON_BN_CLICKED(IDC_BT_READ, &CMiniSatMFCDlg::OnBnClickedBtRead)
END_MESSAGE_MAP()


// CMiniSatMFCDlg メッセージ ハンドラー

BOOL CMiniSatMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CMiniSatMFCDlg::TransformToSudokuCnfText(CString* str)
{
    CString replace_input_text = _T("");
    CString variable_text = _T("");

    int row_index = 1;

    int row_pos = 0;
    auto row_text = str->Tokenize(_T("\r\n"), row_pos);
    while (row_text != "")
    {
        int col_index = 1;
        int cell_pos = 0;
        auto cell_text = row_text.Tokenize(_T(" "), cell_pos);
        while (cell_text != "")
        {
            int val = _ttoi(cell_text);
            if (val >= 1 && val <= 9)
            {
                variable_text.Format(_T("%d%d%d 0\r\n"), row_index, col_index, val);
                replace_input_text += variable_text;
            }

            cell_text = row_text.Tokenize(_T(" "), cell_pos);
            col_index++;
        }

        row_text = str->Tokenize(_T("\r\n"), row_pos);
        row_index++;
    }

    *str = replace_input_text;
}

void CMiniSatMFCDlg::AddSudokuCnfText(CString* str)
{
    CString add_cnf_text = _T("");
    CString cnf = _T("");

    // From 1 to 9 in one cell
    for (size_t i = 1; i <= 9; i++) {
        for (size_t j = 1; j <= 9; j++) {
            for (size_t val = 1; val <= 9; val++)
            {
                cnf.Format(_T("%d%d%d "), i, j, val);
                add_cnf_text += cnf;
            }
            add_cnf_text += _T("0\r\n");
        }
    }

    // From 1 to 9 in one column
    for (size_t val = 1; val <= 9; val++) {
        for (size_t i = 1; i <= 9; i++) {
            for (size_t j = 1; j <= 9; j++) {
                for (size_t jj = j + 1; jj <= 9; jj++)
                {
                    cnf.Format(_T("-%d%d%d -%d%d%d 0\r\n"), j, i, val, jj, i, val);
                    add_cnf_text += cnf;
                }
            }
        }
    }

    // From 1 to 9 in one row
    for (size_t val = 1; val <= 9; val++) {
        for (size_t i = 1; i <= 9; i++) {
            for (size_t j = 1; j <= 9; j++) {
                for (size_t jj = j + 1; jj <= 9; jj++)
                {
                    // row
                    cnf.Format(_T("-%d%d%d -%d%d%d 0\r\n"), i, j, val, i, jj, val);
                    add_cnf_text += cnf;
                }
            }
        }
    }

    // From 1 to 9 in 3×3
    for (size_t start_i = 1; start_i <= 7; start_i += 3)
    {
        for (size_t start_j = 1; start_j <= 7; start_j += 3)
        {
            size_t end_i = start_i + 2;
            size_t end_j = start_j + 2;

            for (size_t val = 1; val <= 9; val++) {
                size_t skip_num = 0;
                for (size_t i0 = start_i; i0 <= end_i; i0++) {
                    for (size_t j0 = start_j; j0 <= end_j; j0++) {

                        skip_num++;
                        size_t count = 0;

                        for (size_t i = start_i; i <= end_i; i++) {
                            for (size_t j = start_j; j <= end_j; j++)
                            {
                                if (count < skip_num)
                                {
                                    count++;
                                    continue;
                                }

                                cnf.Format(_T("-%d%d%d -%d%d%d 0\r\n"), i0, j0, val, i, j, val);
                                add_cnf_text += cnf;
                            }
                        }
                    }
                }
            }
        }
    }

    *str += add_cnf_text;
}

void CMiniSatMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMiniSatMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMiniSatMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMiniSatMFCDlg::OnBnClickedBtCalculate()
{
    UpdateData();

    CString output_text = _T("");
    auto ExitFunc = [&]() {
        m_outputString = output_text;
        UpdateData(FALSE);

        CEdit* output_edit = (CEdit*)GetDlgItem(IDC_EDIT_OUTPUT);
        output_edit->LineScroll(999);
    };

    m_inputString.MakeLower();

    auto first_six_str = m_inputString.Left(6);
    auto first_ten_str = m_inputString.Left(10);
    
    // SUDOKU
    bool is_SUDOKU_cnf = false;
    const CString SUDOKU_STR = _T("sudoku");
    const CString CNF_SUDOKU_STR = _T("cnf sudoku");
    if(first_six_str.CompareNoCase(SUDOKU_STR) == 0)
    {
        is_SUDOKU_cnf = true;

        // trim first one line
        auto temp = m_inputString.TrimLeft(SUDOKU_STR);
        m_inputString = temp.TrimLeft();

        TransformToSudokuCnfText(&m_inputString);
        AddSudokuCnfText(&m_inputString);

        m_inputString = CNF_SUDOKU_STR + _T("\r\n") + m_inputString;
    }
    else if (first_ten_str.CompareNoCase(CNF_SUDOKU_STR) == 0)
    {
        is_SUDOKU_cnf = true;
    }

    try 
    {
        // Extra options:
        Minisat::IntOption    verb("MAIN", "verb", "Verbosity level (0=silent, 1=some, 2=more).", 1, Minisat::IntRange(0, 2));
        Minisat::IntOption    cpu_lim("MAIN", "cpu-lim", "Limit on CPU time allowed in seconds.\n", 0, Minisat::IntRange(0, INT32_MAX));
        Minisat::IntOption    mem_lim("MAIN", "mem-lim", "Limit on memory usage in megabytes.\n", 0, Minisat::IntRange(0, INT32_MAX));
        Minisat::BoolOption   strictp("MAIN", "strict", "Validate DIMACS header during parsing.", false);

        Minisat::Solver S;
        double initial_time = Minisat::cpuTime();

        S.verbosity = verb;

        // Try to set resource limits:
        if (cpu_lim != 0) limitTime(cpu_lim);
        if (mem_lim != 0) limitMemory(mem_lim);

        CString input_text = m_inputString;
        input_text.Replace(_T("\r\n"), _T("\n"));

        // Create input file
        TCHAR full_path[_MAX_PATH];
        ::GetFullPathName(_T("./input.cnf"), _MAX_PATH, full_path, NULL);
        const CString input_path(full_path);
        CStdioFile input_file(input_path, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
        input_file.WriteString(input_text);
        input_file.Close();

        gzFile in = gzopen_w(input_path, "rb");
        if (in == NULL)
        {
            m_outputString = _T("ERROR! Could not open file: ") + input_path;
            UpdateData(FALSE);
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        
        if (S.verbosity > 0) {
            output_text += _T("[ Problem Statistics ]\r\n");
        }

        try
        {
            parse_DIMACS(in, S, (bool)strictp);
        }
        catch (const std::exception&)
        {
            gzclose(in);
            output_text = _T("ERROR: parse_DIMACS\r\n");
            ExitFunc();
            return;
        }
        
        gzclose(in);
        
        CString msg;
        if (S.verbosity > 0) {
            msg.Format(_T("Number of variables:  %12d \r\n"), S.nVars());
            output_text += msg;
            msg.Format(_T("Number of clauses:    %12d \r\n"), S.nClauses());
            output_text += msg;
        }

        double parsed_time = Minisat::cpuTime();
        if (S.verbosity > 0) {
            msg.Format(_T("Parse time:           %12.2f [s] \r\n"), parsed_time - initial_time);
            output_text += msg;
        }

        if (!S.simplify()) 
        {
            if (S.verbosity > 0) {
                output_text += _T("================================================\r\n");
                output_text += _T("Solved by unit propagation\r\n");
                S.printStats();
                output_text += _T("\r\n");
            }
            output_text += _T("UNSATISFIABLE\r\n");

            ExitFunc();
            return;
        }

        Minisat::vec<Minisat::Lit> dummy;
        Minisat::lbool ret = S.solveLimited(dummy);
        if (S.verbosity > 0) {
            S.printStats();
            output_text += _T("\r\n");
        }

        if (ret == Minisat::l_True) {
            output_text += _T("SATISFIABLE\r\n");

            // output
            if (is_SUDOKU_cnf)
            {
                // SUDOKU
                output_text += "------------------\r\n";

                for (size_t i = 1; i <= 9; i++) {
                    for (size_t j = 1; j <= 9; j++) {
                        for (size_t val = 1; val <= 9; val++)
                        {
                            int index = (i * 100) + (j * 10) + val - 1; // -1 because index
                            if (S.model[index] == Minisat::l_True)
                            {
                                msg.Format(_T("%d "), val);
                                output_text += msg;
                                break;
                            }
                        }
                        if (j % 3 == 0)
                            output_text += "| ";
                    }
                    if (i % 3 == 0)
                        output_text += "\r\n------------------";

                    if (i % 9 != 0)
                        output_text += _T("\r\n");
                }
            }
            else
            {
                for (int i = 0; i < S.nVars(); i++)
                {
                    if (S.model[i] != Minisat::l_Undef) {
                        msg.Format(_T("%s%s%d"), (i == 0) ? _T("") : _T(" "), (S.model[i] == Minisat::l_True) ? _T("") : _T("-"), i + 1);
                        output_text += msg;
                    }
                }
                output_text += _T(" 0 \r\n");
            }
        }
        else if (ret == Minisat::l_False)
        {
            output_text += _T("UNSATISFIABLE\r\n");
        }
        else
        {
            output_text += _T("INDETERMINATE\r\n");
        }

        ExitFunc();
        return;
    }
    catch (const std::exception&e) 
    {
        output_text += _T("=============================================\r\n");
        output_text += _T("Error \r\n");
        
        ExitFunc();
        return;
    }
}

void CMiniSatMFCDlg::OnBnClickedBtOpenFile()
{
    CFileDialog dlg(TRUE, _T("*.cnf"), NULL, OFN_EXPLORER, _T("cnf file (*.cnf)|*.cnf|all (*.*)|*.*|"));
    if(dlg.DoModal() == IDOK)
    {
        m_inputFilePath = dlg.GetPathName();
        OnBnClickedBtRead();
    }
}

void CMiniSatMFCDlg::OnBnClickedBtRead()
{
    if(::PathFileExists(m_inputFilePath))
    {
        CStdioFile file;
        file.Open(m_inputFilePath, CFile::typeText);

        m_inputString = _T("");
        CString line;
        while (file.ReadString(line))
        {
            m_inputString += line + _T("\r\n");
        }

        UpdateData(FALSE);
    }
}
