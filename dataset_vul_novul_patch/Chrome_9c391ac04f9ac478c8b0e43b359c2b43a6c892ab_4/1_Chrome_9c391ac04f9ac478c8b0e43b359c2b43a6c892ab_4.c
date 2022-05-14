 void PrintViewManagerBase::OnDidPrintPage(
  const PrintHostMsg_DidPrintPage_Params& params) {
   if (!OpportunisticallyCreatePrintJob(params.document_cookie))
     return;
 
  PrintedDocument* document = print_job_->document();
  if (!document || params.document_cookie != document->cookie()) {
    return;
  }

#if defined(OS_MACOSX)
  const bool metafile_must_be_valid = true;
#else
  const bool metafile_must_be_valid = expecting_first_page_;
  expecting_first_page_ = false;
#endif

  std::unique_ptr<base::SharedMemory> shared_buf;
  if (metafile_must_be_valid) {
    if (!base::SharedMemory::IsHandleValid(params.metafile_data_handle)) {
      NOTREACHED() << "invalid memory handle";
       web_contents()->Stop();
       return;
     }
     shared_buf =
        base::MakeUnique<base::SharedMemory>(params.metafile_data_handle, true);
     if (!shared_buf->Map(params.data_size)) {
       NOTREACHED() << "couldn't map";
       web_contents()->Stop();
      return;
    }
  } else {
    if (base::SharedMemory::IsHandleValid(params.metafile_data_handle)) {
      NOTREACHED() << "unexpected valid memory handle";
      web_contents()->Stop();
      base::SharedMemory::CloseHandle(params.metafile_data_handle);
      return;
     }
   }
 
  std::unique_ptr<PdfMetafileSkia> metafile(
      new PdfMetafileSkia(SkiaDocumentType::PDF));
  if (metafile_must_be_valid) {
    if (!metafile->InitFromData(shared_buf->memory(), params.data_size)) {
      NOTREACHED() << "Invalid metafile header";
      web_contents()->Stop();
      return;
    }
  }
 
 #if defined(OS_WIN)
   print_job_->AppendPrintedPage(params.page_number);
  if (metafile_must_be_valid) {
    scoped_refptr<base::RefCountedBytes> bytes = new base::RefCountedBytes(
         reinterpret_cast<const unsigned char*>(shared_buf->memory()),
        params.data_size);
     document->DebugDumpData(bytes.get(), FILE_PATH_LITERAL(".pdf"));
 
     const auto& settings = document->settings();
    if (settings.printer_is_textonly()) {
      print_job_->StartPdfToTextConversion(bytes, params.page_size);
    } else if ((settings.printer_is_ps2() || settings.printer_is_ps3()) &&
               !base::FeatureList::IsEnabled(
                   features::kDisablePostScriptPrinting)) {
      print_job_->StartPdfToPostScriptConversion(bytes, params.content_area,
                                                 params.physical_offsets,
                                                 settings.printer_is_ps2());
    } else {
      bool print_text_with_gdi = settings.print_text_with_gdi() &&
                                 !settings.printer_is_xps() &&
                                 base::FeatureList::IsEnabled(
                                     features::kGdiTextPrinting);
       print_job_->StartPdfToEmfConversion(
           bytes, params.page_size, params.content_area, print_text_with_gdi);
     }
   }
 #else
  document->SetPage(params.page_number,
                    std::move(metafile),
#if defined(OS_WIN)
                    0.0f /* dummy shrink_factor */,
#endif
                    params.page_size,
                     params.content_area);
 
   ShouldQuitFromInnerMessageLoop();
#endif
}
