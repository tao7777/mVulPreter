bool PrintRenderFrameHelper::PrintPagesNative(blink::WebLocalFrame* frame,
                                              int page_count) {
  const PrintMsg_PrintPages_Params& params = *print_pages_params_;
  const PrintMsg_Print_Params& print_params = params.params;

  std::vector<int> printed_pages = GetPrintedPages(params, page_count);
  if (printed_pages.empty())
    return false;

  PdfMetafileSkia metafile(print_params.printed_doc_type);
  CHECK(metafile.Init());

  for (int page_number : printed_pages) {
    PrintPageInternal(print_params, page_number, page_count, frame, &metafile,
                      nullptr, nullptr);
  }

  FinishFramePrinting();

  metafile.FinishDocument();

#if defined(OS_ANDROID)
  int sequence_number = -1;
  base::FileDescriptor fd;

  Send(new PrintHostMsg_AllocateTempFileForPrinting(routing_id(), &fd,
                                                    &sequence_number));
  if (!SaveToFD(metafile, fd))
    return false;

  Send(new PrintHostMsg_TempFileForPrintingWritten(
      routing_id(), sequence_number, printed_pages.size()));
   return true;
 #else
   PrintHostMsg_DidPrintDocument_Params page_params;
  if (!CopyMetafileDataToReadOnlySharedMem(metafile,
                                           &page_params.metafile_data_handle)) {
     return false;
   }
 
  page_params.data_size = metafile.GetDataSize();
  page_params.document_cookie = print_params.document_cookie;
  Send(new PrintHostMsg_DidPrintDocument(routing_id(), page_params));
  return true;
#endif  // defined(OS_ANDROID)
}
