void PrintWebViewHelper::OnPrintForPrintPreview(
    const DictionaryValue& job_settings) {
  DCHECK(is_preview_);
  if (print_web_view_)
    return;

  if (!render_view()->webview())
    return;
  WebFrame* main_frame = render_view()->webview()->mainFrame();
  if (!main_frame)
    return;

  WebDocument document = main_frame->document();
  WebElement pdf_element = document.getElementById("pdf-viewer");
  if (pdf_element.isNull()) {
    NOTREACHED();
     return;
   }
 
  WebFrame* pdf_frame = pdf_element.document().frame();
  scoped_ptr<PrepareFrameAndViewForPrint> prepare;
  if (!InitPrintSettingsAndPrepareFrame(pdf_frame, &pdf_element, &prepare)) {
    LOG(ERROR) << "Failed to initialize print page settings";
    return;
  }
   if (!UpdatePrintSettings(job_settings, false)) {
     LOG(ERROR) << "UpdatePrintSettings failed";
     DidFinishPrinting(FAIL_PRINT);
     return;
   }
 
   if (!RenderPagesForPrint(pdf_frame, &pdf_element, prepare.get())) {
     LOG(ERROR) << "RenderPagesForPrint failed";
    DidFinishPrinting(FAIL_PRINT);
  }
}
