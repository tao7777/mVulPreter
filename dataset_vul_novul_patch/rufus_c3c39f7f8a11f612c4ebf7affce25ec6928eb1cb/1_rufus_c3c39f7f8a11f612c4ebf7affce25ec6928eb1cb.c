LONG ValidateSignature(HWND hDlg, const char* path)
{
	LONG r;
	WINTRUST_DATA trust_data = { 0 };
	WINTRUST_FILE_INFO trust_file = { 0 };
	GUID guid_generic_verify =	// WINTRUST_ACTION_GENERIC_VERIFY_V2
		{ 0xaac56b, 0xcd44, 0x11d0,{ 0x8c, 0xc2, 0x0, 0xc0, 0x4f, 0xc2, 0x95, 0xee } };
	char *signature_name;
	size_t i, len;

	signature_name = GetSignatureName(path);
	if (signature_name == NULL) {
		uprintf("PKI: Could not get signature name");
		MessageBoxExU(hDlg, lmprintf(MSG_284), lmprintf(MSG_283), MB_OK | MB_ICONERROR | MB_IS_RTL, selected_langid);
		return TRUST_E_NOSIGNATURE;
	}
	for (i = 0; i < ARRAYSIZE(cert_name); i++) {
		len = strlen(cert_name[i]);
		if (strncmp(signature_name, cert_name[i], len) == 0) {
			if ((len >= strlen(signature_name)) || isspace(signature_name[len]))
				break;
		}
	}
	if (i >= ARRAYSIZE(cert_name)) {
		uprintf("PKI: Signature '%s' is unexpected...", signature_name);
		if (MessageBoxExU(hDlg, lmprintf(MSG_285, signature_name), lmprintf(MSG_283),
			MB_YESNO | MB_ICONWARNING | MB_IS_RTL, selected_langid) != IDYES)
			return TRUST_E_EXPLICIT_DISTRUST;
	}

	trust_file.cbStruct = sizeof(trust_file);
	trust_file.pcwszFilePath = utf8_to_wchar(path);
	if (trust_file.pcwszFilePath == NULL) {
		uprintf("PKI: Unable to convert '%s' to UTF16", path);
		return ERROR_SEVERITY_ERROR | FAC(FACILITY_CERT) | ERROR_NOT_ENOUGH_MEMORY;
 	}
 
 	trust_data.cbStruct = sizeof(trust_data);
	trust_data.dwUIChoice = WTD_UI_ALL;
 	trust_data.fdwRevocationChecks = WTD_REVOKE_WHOLECHAIN;
	trust_data.dwProvFlags = WTD_REVOCATION_CHECK_CHAIN | 0x400;
	trust_data.dwUnionChoice = WTD_CHOICE_FILE;
	trust_data.pFile = &trust_file;
 
 	r = WinVerifyTrust(NULL, &guid_generic_verify, &trust_data);
 	safe_free(trust_file.pcwszFilePath);
 
 	return r;
 }
