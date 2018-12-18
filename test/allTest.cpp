//ComboBox.cpp
//#include "stdafx.h"
#include "resource.h"

LRESULT CALLBACK Dialog(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// TODO: Place code here.
	DialogBox(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)Dialog);
	return 0;
}

LRESULT CALLBACK Dialog(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		UINT uSender;
		uSender = LOWORD(wParam);
		HWND hWndComboBox;
		hWndComboBox = GetDlgItem(hWnd, IDC_COMBO1);
		TCHAR szBuff[200];
		ZeroMemory(szBuff, sizeof(szBuff));
		switch (uSender)
		{
			//CB_ADDSTRING是在最后添加数据
		case IDC_BUTTON1:
			GetDlgItemText(hWnd, IDC_EDIT1, szBuff, sizeof(szBuff));
			SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)szBuff);
			break;

			//CB_ADDSTRING是在指定位置添加数据
		case IDC_BUTTON2:
			GetDlgItemText(hWnd, IDC_EDIT1, szBuff, sizeof(szBuff));
			SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)szBuff);
			break;

		case IDC_BUTTON3:
			SendMessage(hWndComboBox, CB_RESETCONTENT, 0, 0);
			break;

		case IDC_BUTTON4:
			UINT uCount;
			uCount = SendMessage(hWndComboBox, CB_GETCOUNT, 0, 0);
			SetDlgItemInt(hWnd, IDC_EDIT2, uCount, TRUE);
			break;

		case IDC_BUTTON5:
			UINT uSelect;
			uSelect = GetDlgItemInt(hWnd, IDC_EDIT2, NULL, TRUE);
			SendMessage(hWndComboBox, CB_GETLBTEXT, uSelect, (LPARAM)szBuff);
			MessageBox(hWnd, szBuff, "SHOW", MB_OK | MB_ICONINFORMATION);
			break;

		case IDOK:
			EndDialog(hWnd, lParam);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, lParam);
		break;
	}
	return FALSE;
}
