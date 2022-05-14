rdpCredssp* credssp_new(freerdp* instance, rdpTransport* transport, rdpSettings* settings)
{
	rdpCredssp* credssp;

	credssp = (rdpCredssp*) malloc(sizeof(rdpCredssp));
	ZeroMemory(credssp, sizeof(rdpCredssp));

	if (credssp != NULL)
	{
		HKEY hKey;
		LONG status;
		DWORD dwType;
		DWORD dwSize;

		credssp->instance = instance;
		credssp->settings = settings;
		credssp->server = settings->ServerMode;
		credssp->transport = transport;
		credssp->send_seq_num = 0;
		credssp->recv_seq_num = 0;
 		ZeroMemory(&credssp->negoToken, sizeof(SecBuffer));
 		ZeroMemory(&credssp->pubKeyAuth, sizeof(SecBuffer));
 		ZeroMemory(&credssp->authInfo, sizeof(SecBuffer));
		SecInvalidateHandle(&credssp->context);
 
 		if (credssp->server)
 		{
			status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FreeRDP\\Server"),
					0, KEY_READ | KEY_WOW64_64KEY, &hKey);

			if (status == ERROR_SUCCESS)
			{
				status = RegQueryValueEx(hKey, _T("SspiModule"), NULL, &dwType, NULL, &dwSize);

				if (status == ERROR_SUCCESS)
				{
					credssp->SspiModule = (LPTSTR) malloc(dwSize + sizeof(TCHAR));

					status = RegQueryValueEx(hKey, _T("SspiModule"), NULL, &dwType,
							(BYTE*) credssp->SspiModule, &dwSize);

					if (status == ERROR_SUCCESS)
					{
						_tprintf(_T("Using SSPI Module: %s\n"), credssp->SspiModule);
						RegCloseKey(hKey);
					}
				}
			}
		}
	}

	return credssp;
}
