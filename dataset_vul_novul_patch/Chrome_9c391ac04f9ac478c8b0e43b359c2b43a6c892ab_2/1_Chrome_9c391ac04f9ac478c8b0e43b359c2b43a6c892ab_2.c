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
 
  scoped_refptr<base::RefCountedBytes> data_bytes =
      GetDataFromHandle(params.metafile_data_handle, params.data_size);
  if (!data_bytes || !data_bytes->size())
    return;
  print_preview_ui->SetPrintPreviewDataForIndex(COMPLETE_PREVIEW_DOCUMENT_INDEX,
                                                std::move(data_bytes));
  print_preview_ui->OnPreviewDataIsAvailable(
      params.expected_pages_count, params.preview_request_id);
 }
