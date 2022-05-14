void ChromeMockRenderThread::OnUpdatePrintSettings(
    int document_cookie,
    const base::DictionaryValue& job_settings,
    PrintMsg_PrintPages_Params* params) {
  std::string dummy_string;
  int margins_type = 0;
  if (!job_settings.GetBoolean(printing::kSettingLandscape, NULL) ||
      !job_settings.GetBoolean(printing::kSettingCollate, NULL) ||
      !job_settings.GetInteger(printing::kSettingColor, NULL) ||
      !job_settings.GetBoolean(printing::kSettingPrintToPDF, NULL) ||
      !job_settings.GetBoolean(printing::kIsFirstRequest, NULL) ||
       !job_settings.GetString(printing::kSettingDeviceName, &dummy_string) ||
       !job_settings.GetInteger(printing::kSettingDuplexMode, NULL) ||
       !job_settings.GetInteger(printing::kSettingCopies, NULL) ||
      !job_settings.GetInteger(printing::kPreviewUIID, NULL) ||
       !job_settings.GetInteger(printing::kPreviewRequestID, NULL) ||
       !job_settings.GetInteger(printing::kSettingMarginsType, &margins_type)) {
     return;
   }
 
  const ListValue* page_range_array;
  printing::PageRanges new_ranges;
  if (job_settings.GetList(printing::kSettingPageRange, &page_range_array)) {
    for (size_t index = 0; index < page_range_array->GetSize(); ++index) {
      const base::DictionaryValue* dict;
      if (!page_range_array->GetDictionary(index, &dict))
        continue;
      printing::PageRange range;
      if (!dict->GetInteger(printing::kSettingPageRangeFrom, &range.from) ||
          !dict->GetInteger(printing::kSettingPageRangeTo, &range.to)) {
        continue;
       }
      // Page numbers are 1-based in the dictionary.
      // Page numbers are 0-based for the printing context.
      range.from--;
      range.to--;
      new_ranges.push_back(range);
     }
   }
  std::vector<int> pages(printing::PageRange::GetPages(new_ranges));
  printer_->UpdateSettings(document_cookie, params, pages, margins_type);
}

MockPrinter* ChromeMockRenderThread::printer() {
  return printer_.get();
 }
