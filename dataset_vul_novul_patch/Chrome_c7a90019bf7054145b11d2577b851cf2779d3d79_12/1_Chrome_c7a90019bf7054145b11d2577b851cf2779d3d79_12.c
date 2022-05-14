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
