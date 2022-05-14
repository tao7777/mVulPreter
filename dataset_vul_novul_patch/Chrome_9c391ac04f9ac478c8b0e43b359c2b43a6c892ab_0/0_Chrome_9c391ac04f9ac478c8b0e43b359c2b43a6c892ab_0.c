void PrintPreviewMessageHandler::OnDidPreviewPage(
    const PrintHostMsg_DidPreviewPage_Params& params) {
  int page_number = params.page_number;
  if (page_number < FIRST_PAGE_INDEX || !params.data_size)
    return;

  PrintPreviewUI* print_preview_ui = GetPrintPreviewUI();
   if (!print_preview_ui)
     return;
 
  if (IsOopifEnabled() && print_preview_ui->source_is_modifiable()) {
    auto* client = PrintCompositeClient::FromWebContents(web_contents());
    DCHECK(client);

    // Use utility process to convert skia metafile to pdf.
    client->DoComposite(
        params.metafile_data_handle, params.data_size,
        base::BindOnce(&PrintPreviewMessageHandler::OnCompositePdfPageDone,
                       weak_ptr_factory_.GetWeakPtr(), params.page_number,
                       params.preview_request_id));
  } else {
    NotifyUIPreviewPageReady(
        page_number, params.preview_request_id,
        GetDataFromHandle(params.metafile_data_handle, params.data_size));
  }
 }
