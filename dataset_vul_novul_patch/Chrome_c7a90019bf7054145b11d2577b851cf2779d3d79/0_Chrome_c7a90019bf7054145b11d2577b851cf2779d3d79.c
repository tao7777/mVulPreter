bool PrintDialogGtk::UpdateSettings(const DictionaryValue& settings,
                                    const printing::PageRanges& ranges) {
  bool collate;
  int color;
  bool landscape;
  bool print_to_pdf;
  int copies;
  int duplex_mode;
  std::string device_name;

  if (!settings.GetBoolean(printing::kSettingLandscape, &landscape) ||
      !settings.GetBoolean(printing::kSettingCollate, &collate) ||
      !settings.GetInteger(printing::kSettingColor, &color) ||
      !settings.GetBoolean(printing::kSettingPrintToPDF, &print_to_pdf) ||
      !settings.GetInteger(printing::kSettingDuplexMode, &duplex_mode) ||
      !settings.GetInteger(printing::kSettingCopies, &copies) ||
      !settings.GetString(printing::kSettingDeviceName, &device_name)) {
     return false;
   }
 
  if (!gtk_settings_)
    gtk_settings_ = gtk_print_settings_new();

   if (!print_to_pdf) {
     scoped_ptr<GtkPrinterList> printer_list(new GtkPrinterList);
     printer_ = printer_list->GetPrinterWithName(device_name.c_str());
     if (printer_) {
       g_object_ref(printer_);
       gtk_print_settings_set_printer(gtk_settings_,
                                      gtk_printer_get_name(printer_));
      if (!page_setup_) {
        page_setup_ = gtk_printer_get_default_page_size(printer_);
      }
     }
    if (!page_setup_)
      page_setup_ = gtk_page_setup_new();

     gtk_print_settings_set_n_copies(gtk_settings_, copies);
     gtk_print_settings_set_collate(gtk_settings_, collate);
 
    const char* color_mode;
    switch (color) {
      case printing::COLOR:
        color_mode = kColor;
        break;
      case printing::CMYK:
        color_mode = kCMYK;
        break;
      default:
        color_mode = kGrayscale;
        break;
    }
    gtk_print_settings_set(gtk_settings_, kCUPSColorModel, color_mode);

    if (duplex_mode != printing::UNKNOWN_DUPLEX_MODE) {
      const char* cups_duplex_mode = NULL;
      switch (duplex_mode) {
        case printing::LONG_EDGE:
          cups_duplex_mode = kDuplexNoTumble;
          break;
        case printing::SHORT_EDGE:
          cups_duplex_mode = kDuplexTumble;
          break;
        case printing::SIMPLEX:
          cups_duplex_mode = kDuplexNone;
          break;
        default:  // UNKNOWN_DUPLEX_MODE
          NOTREACHED();
          break;
      }
      gtk_print_settings_set(gtk_settings_, kCUPSDuplex, cups_duplex_mode);
    }
  }

  gtk_print_settings_set_orientation(
      gtk_settings_,
      landscape ? GTK_PAGE_ORIENTATION_LANDSCAPE :
                  GTK_PAGE_ORIENTATION_PORTRAIT);

  InitPrintSettings(ranges);
  return true;
}
