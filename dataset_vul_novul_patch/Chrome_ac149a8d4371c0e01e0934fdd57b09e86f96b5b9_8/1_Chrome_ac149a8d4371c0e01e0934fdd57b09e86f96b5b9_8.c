static int _hid_wcslen(WCHAR *str)
{
	int i = 0;
	while (str[i] && (str[i] != 0x409)) {
		i++;
	}
	return i;
}
