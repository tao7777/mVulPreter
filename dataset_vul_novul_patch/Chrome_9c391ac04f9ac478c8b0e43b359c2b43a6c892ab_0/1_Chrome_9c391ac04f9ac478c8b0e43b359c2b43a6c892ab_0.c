void PrintPreviewMessageHandler::OnDidPreviewPage(
    const PrintHostMsg_DidPreviewPage_Params& params) {
  int page_number = params.page_number;
  if (page_number < FIRST_PAGE_INDEX || !params.data_size)
    return;

  PrintPreviewUI* print_preview_ui = GetPrintPreviewUI();
   if (!print_preview_ui)
     return;
 
  scoped_refptr<base::RefCountedBytes> data_bytes =
      GetDataFromHandle(params.metafile_data_handle, params.data_size);
  DCHECK(data_bytes);
  print_preview_ui->SetPrintPreviewDataForIndex(page_number,
                                                std::move(data_bytes));
  print_preview_ui->OnDidPreviewPage(page_number, params.preview_request_id);
 }
