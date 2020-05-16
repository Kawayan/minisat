#include "pch.h"
#include "CnfSusoku.h"

void CnfSusoku::TransformToCnfText(CString* str)
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

void CnfSusoku::AddCnfText(CString* str)
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

    // From 1 to 9 in 3~3
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
