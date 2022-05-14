PrintingContextCairo::PrintingContextCairo(const std::string& app_locale)
#if defined(OS_CHROMEOS)
    : PrintingContext(app_locale) {
#else
    : PrintingContext(app_locale),
      print_dialog_(NULL) {
#endif
}

PrintingContextCairo::~PrintingContextCairo() {
  ReleaseContext();

#if !defined(OS_CHROMEOS)
  if (print_dialog_)
    print_dialog_->ReleaseDialog();
#endif
}

#if !defined(OS_CHROMEOS)
void PrintingContextCairo::SetCreatePrintDialogFunction(
    PrintDialogGtkInterface* (*create_dialog_func)(
        PrintingContextCairo* context)) {
  DCHECK(create_dialog_func);
  DCHECK(!create_dialog_func_);
  create_dialog_func_ = create_dialog_func;
}

void PrintingContextCairo::PrintDocument(const Metafile* metafile) {
  DCHECK(print_dialog_);
  DCHECK(metafile);
  print_dialog_->PrintDocument(metafile, document_name_);
}
#endif  // !defined(OS_CHROMEOS)

void PrintingContextCairo::AskUserForSettings(
    gfx::NativeView parent_view,
    int max_pages,
    bool has_selection,
    PrintSettingsCallback* callback) {
#if defined(OS_CHROMEOS)
  callback->Run(OK);
#else
  print_dialog_->ShowDialog(callback);
#endif  // defined(OS_CHROMEOS)
}

PrintingContext::Result PrintingContextCairo::UseDefaultSettings() {
  DCHECK(!in_print_job_);

  ResetSettings();
#if defined(OS_CHROMEOS)
  int dpi = 300;
  gfx::Size physical_size_device_units;
  gfx::Rect printable_area_device_units;
  int32_t width = 0;
  int32_t height = 0;
  UErrorCode error = U_ZERO_ERROR;
  ulocdata_getPaperSize(app_locale_.c_str(), &height, &width, &error);
  if (error != U_ZERO_ERROR) {
    LOG(WARNING) << "ulocdata_getPaperSize failed, using 8.5 x 11, error: "
                 << error;
    width = static_cast<int>(8.5 * dpi);
    height = static_cast<int>(11 * dpi);
  } else {
    width = static_cast<int>(ConvertUnitDouble(width, 25.4, 1.0) * dpi);
    height = static_cast<int>(ConvertUnitDouble(height, 25.4, 1.0) * dpi);
  }

  physical_size_device_units.SetSize(width, height);
  printable_area_device_units.SetRect(
      static_cast<int>(PrintSettingsInitializerGtk::kLeftMarginInInch * dpi),
      static_cast<int>(PrintSettingsInitializerGtk::kTopMarginInInch * dpi),
      width - (PrintSettingsInitializerGtk::kLeftMarginInInch +
          PrintSettingsInitializerGtk::kRightMarginInInch) * dpi,
      height - (PrintSettingsInitializerGtk::kTopMarginInInch +
          PrintSettingsInitializerGtk::kBottomMarginInInch) * dpi);

  settings_.set_dpi(dpi);
  settings_.SetPrinterPrintableArea(physical_size_device_units,
                                    printable_area_device_units,
                                    dpi);
#else
  if (!print_dialog_) {
    print_dialog_ = create_dialog_func_(this);
    print_dialog_->AddRefToDialog();
  }
  print_dialog_->UseDefaultSettings();
#endif  // defined(OS_CHROMEOS)

  return OK;
}

PrintingContext::Result PrintingContextCairo::UpdatePrinterSettings(
    const DictionaryValue& job_settings, const PageRanges& ranges) {
#if defined(OS_CHROMEOS)
  bool landscape = false;

  if (!job_settings.GetBoolean(kSettingLandscape, &landscape))
    return OnError();

  settings_.SetOrientation(landscape);
  settings_.ranges = ranges;

  return OK;
 #else
   DCHECK(!in_print_job_);
 
   if (!print_dialog_->UpdateSettings(job_settings, ranges))
     return OnError();
 
  return OK;
#endif
}

PrintingContext::Result PrintingContextCairo::InitWithSettings(
    const PrintSettings& settings) {
  DCHECK(!in_print_job_);

  settings_ = settings;

  return OK;
}

PrintingContext::Result PrintingContextCairo::NewDocument(
    const string16& document_name) {
  DCHECK(!in_print_job_);
  in_print_job_ = true;

#if !defined(OS_CHROMEOS)
  document_name_ = document_name;
#endif  // !defined(OS_CHROMEOS)

  return OK;
}

PrintingContext::Result PrintingContextCairo::NewPage() {
  if (abort_printing_)
    return CANCEL;
  DCHECK(in_print_job_);


  return OK;
}

PrintingContext::Result PrintingContextCairo::PageDone() {
  if (abort_printing_)
    return CANCEL;
  DCHECK(in_print_job_);


  return OK;
}

PrintingContext::Result PrintingContextCairo::DocumentDone() {
  if (abort_printing_)
    return CANCEL;
  DCHECK(in_print_job_);

  ResetSettings();
  return OK;
}

void PrintingContextCairo::Cancel() {
  abort_printing_ = true;
  in_print_job_ = false;
}

void PrintingContextCairo::ReleaseContext() {
}

gfx::NativeDrawingContext PrintingContextCairo::context() const {
  return NULL;
}

}  // namespace printing
