void PrintSettingsInitializerWin::InitPrintSettings(
    HDC hdc,
    const DEVMODE& dev_mode,
    const PageRanges& new_ranges,
    const std::wstring& new_device_name,
    bool print_selection_only,
    PrintSettings* print_settings) {
  DCHECK(hdc);
  DCHECK(print_settings);
 
   print_settings->set_printer_name(dev_mode.dmDeviceName);
   print_settings->set_device_name(new_device_name);
  print_settings->ranges = new_ranges;
   print_settings->set_landscape(dev_mode.dmOrientation == DMORIENT_LANDSCAPE);
   print_settings->selection_only = print_selection_only;
 
  int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
  print_settings->set_dpi(dpi);
  const int kAlphaCaps = SB_CONST_ALPHA | SB_PIXEL_ALPHA;
  print_settings->set_supports_alpha_blend(
    (GetDeviceCaps(hdc, SHADEBLENDCAPS) & kAlphaCaps) == kAlphaCaps);
  DCHECK_EQ(dpi, GetDeviceCaps(hdc, LOGPIXELSY));

  DCHECK_EQ(GetDeviceCaps(hdc, SCALINGFACTORX), 0);
  DCHECK_EQ(GetDeviceCaps(hdc, SCALINGFACTORY), 0);

  gfx::Size physical_size_device_units(GetDeviceCaps(hdc, PHYSICALWIDTH),
                                       GetDeviceCaps(hdc, PHYSICALHEIGHT));
  gfx::Rect printable_area_device_units(GetDeviceCaps(hdc, PHYSICALOFFSETX),
                                        GetDeviceCaps(hdc, PHYSICALOFFSETY),
                                        GetDeviceCaps(hdc, HORZRES),
                                        GetDeviceCaps(hdc, VERTRES));

  print_settings->SetPrinterPrintableArea(physical_size_device_units,
                                          printable_area_device_units,
                                          dpi);
}
