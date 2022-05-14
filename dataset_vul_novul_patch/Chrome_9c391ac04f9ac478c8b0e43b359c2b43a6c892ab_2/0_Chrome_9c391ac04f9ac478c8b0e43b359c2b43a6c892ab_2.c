 void PrintPreviewMessageHandler::OnMetafileReadyForPrinting(
    const PrintHostMsg_DidPreviewDocument_Params& params) {
  StopWorker(params.document_cookie);

  if (params.expected_pages_count <= 0) {
    NOTREACHED();
    return;
  }

  PrintPreviewUI* print_preview_ui = GetPrintPreviewUI();
   if (!print_preview_ui)
     return;
 
  if (IsOopifEnabled() && print_preview_ui->source_is_modifiable()) {
    auto* client = PrintCompositeClient::FromWebContents(web_contents());
    DCHECK(client);

    client->DoComposite(
        params.metafile_data_handle, params.data_size,
        base::BindOnce(&PrintPreviewMessageHandler::OnCompositePdfDocumentDone,
                       weak_ptr_factory_.GetWeakPtr(),
                       params.expected_pages_count, params.preview_request_id));
  } else {
    NotifyUIPreviewDocumentReady(
        params.expected_pages_count, params.preview_request_id,
        GetDataFromHandle(params.metafile_data_handle, params.data_size));
  }
 }
