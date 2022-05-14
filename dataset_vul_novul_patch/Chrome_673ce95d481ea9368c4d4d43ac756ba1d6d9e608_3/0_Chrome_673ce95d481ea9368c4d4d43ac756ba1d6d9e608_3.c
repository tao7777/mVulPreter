bool PrintRenderFrameHelper::PrintPagesNative(blink::WebLocalFrame* frame,
                                              int page_count) {
  const PrintMsg_PrintPages_Params& params = *print_pages_params_;
  const PrintMsg_Print_Params& print_params = params.params;

  std::vector<int> printed_pages = GetPrintedPages(params, page_count);
  if (printed_pages.empty())
    return false;

  PdfMetafileSkia metafile(print_params.printed_doc_type);
  CHECK(metafile.Init());

  PrintHostMsg_DidPrintDocument_Params page_params;
  PrintPageInternal(print_params, printed_pages[0], page_count, frame,
                    &metafile, &page_params.page_size,
                    &page_params.content_area);
  for (size_t i = 1; i < printed_pages.size(); ++i) {
    PrintPageInternal(print_params, printed_pages[i], page_count, frame,
                      &metafile, nullptr, nullptr);
  }

  FinishFramePrinting();
 
   metafile.FinishDocument();
 
  if (!CopyMetafileDataToReadOnlySharedMem(metafile,
                                           &page_params.metafile_data_handle)) {
     return false;
   }
 
  page_params.data_size = metafile.GetDataSize();
  page_params.document_cookie = print_params.document_cookie;
#if defined(OS_WIN)
  page_params.physical_offsets = printer_printable_area_.origin();
#endif
  Send(new PrintHostMsg_DidPrintDocument(routing_id(), page_params));
  return true;
}
