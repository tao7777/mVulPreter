static int hid_init(int sub_api, struct libusb_context *ctx)
{
	DLL_LOAD(hid.dll, HidD_GetAttributes, TRUE);
	DLL_LOAD(hid.dll, HidD_GetHidGuid, TRUE);
	DLL_LOAD(hid.dll, HidD_GetPreparsedData, TRUE);
	DLL_LOAD(hid.dll, HidD_FreePreparsedData, TRUE);
	DLL_LOAD(hid.dll, HidD_GetManufacturerString, TRUE);
	DLL_LOAD(hid.dll, HidD_GetProductString, TRUE);
	DLL_LOAD(hid.dll, HidD_GetSerialNumberString, TRUE);
	DLL_LOAD(hid.dll, HidP_GetCaps, TRUE);
	DLL_LOAD(hid.dll, HidD_SetNumInputBuffers, TRUE);
	DLL_LOAD(hid.dll, HidD_SetFeature, TRUE);
	DLL_LOAD(hid.dll, HidD_GetFeature, TRUE);
	DLL_LOAD(hid.dll, HidD_GetPhysicalDescriptor, TRUE);
	DLL_LOAD(hid.dll, HidD_GetInputReport, FALSE);
	DLL_LOAD(hid.dll, HidD_SetOutputReport, FALSE);
	DLL_LOAD(hid.dll, HidD_FlushQueue, TRUE);
	DLL_LOAD(hid.dll, HidP_GetValueCaps, TRUE);
	api_hid_available = true;
	return LIBUSB_SUCCESS;
}
