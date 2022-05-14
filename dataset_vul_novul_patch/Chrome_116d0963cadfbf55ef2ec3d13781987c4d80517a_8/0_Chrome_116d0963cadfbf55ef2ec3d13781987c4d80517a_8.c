void PrintPreviewHandler::HandleGetPreview(const ListValue* args) {
  DCHECK_EQ(3U, args->GetSize());
  scoped_ptr<DictionaryValue> settings(GetSettingsDictionary(args));
  if (!settings.get())
    return;
  int request_id = -1;
  if (!settings->GetInteger(printing::kPreviewRequestID, &request_id))
    return;

  PrintPreviewUI* print_preview_ui = static_cast<PrintPreviewUI*>(
      web_ui()->GetController());
  print_preview_ui->OnPrintPreviewRequest(request_id);
  settings->SetInteger(printing::kPreviewUIID,
                       print_preview_ui->GetIDForPrintPreviewUI());
 
   ++regenerate_preview_request_count_;

  TabContents* initiator_tab = GetInitiatorTab();
  if (!initiator_tab) {
    ReportUserActionHistogram(INITIATOR_TAB_CLOSED);
    print_preview_ui->OnClosePrintPreviewTab();
    return;
  }

  bool display_header_footer = false;
  if (!settings->GetBoolean(printing::kSettingHeaderFooterEnabled,
                            &display_header_footer)) {
    NOTREACHED();
  }
  if (display_header_footer) {
    settings->SetString(printing::kSettingHeaderFooterTitle,
                        initiator_tab->web_contents()->GetTitle());
    std::string url;
    NavigationEntry* entry =
        initiator_tab->web_contents()->GetController().GetActiveEntry();
    if (entry)
      url = entry->GetVirtualURL().spec();
    settings->SetString(printing::kSettingHeaderFooterURL, url);
  }

  bool generate_draft_data = false;
  bool success = settings->GetBoolean(printing::kSettingGenerateDraftData,
                                      &generate_draft_data);
  DCHECK(success);

  if (!generate_draft_data) {
    double draft_page_count_double = -1;
    success = args->GetDouble(1, &draft_page_count_double);
    DCHECK(success);
    int draft_page_count = static_cast<int>(draft_page_count_double);

    bool preview_modifiable = false;
    success = args->GetBoolean(2, &preview_modifiable);
    DCHECK(success);

    if (draft_page_count != -1 && preview_modifiable &&
        print_preview_ui->GetAvailableDraftPageCount() != draft_page_count) {
      settings->SetBoolean(printing::kSettingGenerateDraftData, true);
    }
  }

  VLOG(1) << "Print preview request start";
  RenderViewHost* rvh = initiator_tab->web_contents()->GetRenderViewHost();
  rvh->Send(new PrintMsg_PrintPreview(rvh->GetRoutingID(), *settings));
}
