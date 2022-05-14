INT_PTR CALLBACK NewVersionCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char cmdline[] = APPLICATION_NAME " -w 150";
	static char* filepath = NULL;
	static int download_status = 0;
	LONG i;
	HWND hNotes;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	HFONT hyperlink_font = NULL;
	EXT_DECL(dl_ext, NULL, __VA_GROUP__("*.exe"), __VA_GROUP__(lmprintf(MSG_037)));

	switch (message) {
	case WM_INITDIALOG:
		apply_localization(IDD_NEW_VERSION, hDlg);
		download_status = 0;
		SetTitleBarIcon(hDlg);
		CenterDialog(hDlg);
		update_original_proc = (WNDPROC)SetWindowLongPtr(hDlg, GWLP_WNDPROC, (LONG_PTR)update_subclass_callback);
		hNotes = GetDlgItem(hDlg, IDC_RELEASE_NOTES);
		SendMessage(hNotes, EM_AUTOURLDETECT, 1, 0);
		SendMessageA(hNotes, EM_SETTEXTEX, (WPARAM)&friggin_microsoft_unicode_amateurs, (LPARAM)update.release_notes);
		SendMessage(hNotes, EM_SETSEL, -1, -1);
		SendMessage(hNotes, EM_SETEVENTMASK, 0, ENM_LINK);
		SetWindowTextU(GetDlgItem(hDlg, IDC_YOUR_VERSION), lmprintf(MSG_018,
			rufus_version[0], rufus_version[1], rufus_version[2]));
		SetWindowTextU(GetDlgItem(hDlg, IDC_LATEST_VERSION), lmprintf(MSG_019,
			update.version[0], update.version[1], update.version[2]));
		SetWindowTextU(GetDlgItem(hDlg, IDC_DOWNLOAD_URL), update.download_url);
		SendMessage(GetDlgItem(hDlg, IDC_PROGRESS), PBM_SETRANGE, 0, (MAX_PROGRESS<<16) & 0xFFFF0000);
		if (update.download_url == NULL)
			EnableWindow(GetDlgItem(hDlg, IDC_DOWNLOAD), FALSE);
		break;
	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam != GetDlgItem(hDlg, IDC_WEBSITE))
			return FALSE;
		SetBkMode((HDC)wParam, TRANSPARENT);
		CreateStaticFont((HDC)wParam, &hyperlink_font);
		SelectObject((HDC)wParam, hyperlink_font);
		SetTextColor((HDC)wParam, RGB(0,0,125));	// DARK_BLUE
		return (INT_PTR)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCLOSE:
		case IDCANCEL:
			if (download_status != 1) {
				reset_localization(IDD_NEW_VERSION);
				safe_free(filepath);
				EndDialog(hDlg, LOWORD(wParam));
			}
			return (INT_PTR)TRUE;
		case IDC_WEBSITE:
			ShellExecuteA(hDlg, "open", RUFUS_URL, NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDC_DOWNLOAD:	// Also doubles as abort and launch function
			switch(download_status) {
			case 1:		// Abort
				FormatStatus = ERROR_SEVERITY_ERROR|FAC(FACILITY_STORAGE)|ERROR_CANCELLED;
				download_status = 0;
				break;
 			case 2:		// Launch newer version and close this one
 				Sleep(1000);	// Add a delay on account of antivirus scanners
 
				if (ValidateSignature(hDlg, filepath) != NO_ERROR)
 					break;
 
 				memset(&si, 0, sizeof(si));
 				memset(&pi, 0, sizeof(pi));
				si.cb = sizeof(si);
				if (!CreateProcessU(filepath, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
					PrintInfo(0, MSG_214);
					uprintf("Failed to launch new application: %s\n", WindowsErrorString());
				} else {
					PrintInfo(0, MSG_213);
					PostMessage(hDlg, WM_COMMAND, (WPARAM)IDCLOSE, 0);
					PostMessage(hMainDialog, WM_CLOSE, 0, 0);
				}
				break;
			default:	// Download
				if (update.download_url == NULL) {
					uprintf("Could not get download URL\n");
					break;
				}
				for (i=(int)strlen(update.download_url); (i>0)&&(update.download_url[i]!='/'); i--);
				dl_ext.filename = &update.download_url[i+1];
				filepath = FileDialog(TRUE, app_dir, &dl_ext, OFN_NOCHANGEDIR);
				if (filepath == NULL) {
					uprintf("Could not get save path\n");
					break;
				}
				SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDC_DOWNLOAD), TRUE);
				DownloadFileThreaded(update.download_url, filepath, hDlg);
				break;
			}
			return (INT_PTR)TRUE;
		}
		break;
	case UM_PROGRESS_INIT:
		EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
		SetWindowTextU(GetDlgItem(hDlg, IDC_DOWNLOAD), lmprintf(MSG_038));
		FormatStatus = 0;
		download_status = 1;
		return (INT_PTR)TRUE;
	case UM_PROGRESS_EXIT:
		EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
		if (wParam) {
			SetWindowTextU(GetDlgItem(hDlg, IDC_DOWNLOAD), lmprintf(MSG_039));
			download_status = 2;
		} else {
			SetWindowTextU(GetDlgItem(hDlg, IDC_DOWNLOAD), lmprintf(MSG_040));
			download_status = 0;
		}
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}
