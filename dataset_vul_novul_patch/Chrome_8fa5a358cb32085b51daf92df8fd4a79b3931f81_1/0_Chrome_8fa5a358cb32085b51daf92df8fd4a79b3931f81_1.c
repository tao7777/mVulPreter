 void PrintWebViewHelper::OnPrintForPrintPreview(
     const base::DictionaryValue& job_settings) {
  CHECK_LE(ipc_nesting_level_, 1);
   if (prep_frame_view_)
     return;

  if (!render_view()->GetWebView())
    return;
  blink::WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
  if (!main_frame)
    return;

  blink::WebDocument document = main_frame->document();
  blink::WebElement pdf_element = document.getElementById("pdf-viewer");
  if (pdf_element.isNull()) {
    NOTREACHED();
    return;
  }

  blink::WebLocalFrame* plugin_frame = pdf_element.document().frame();
  blink::WebElement plugin_element = pdf_element;
  if (pdf_element.hasHTMLTagName("iframe")) {
    plugin_frame = blink::WebLocalFrame::fromFrameOwnerElement(pdf_element);
    plugin_element = delegate_->GetPdfElement(plugin_frame);
    if (plugin_element.isNull()) {
      NOTREACHED();
      return;
    }
  }

  base::AutoReset<bool> set_printing_flag(&print_for_preview_, true);

  if (!UpdatePrintSettings(plugin_frame, plugin_element, job_settings)) {
    LOG(ERROR) << "UpdatePrintSettings failed";
    DidFinishPrinting(FAIL_PRINT);
    return;
  }

  PrintMsg_Print_Params& print_params = print_pages_params_->params;
  print_params.printable_area = gfx::Rect(print_params.page_size);

  if (!RenderPagesForPrint(plugin_frame, plugin_element)) {
    LOG(ERROR) << "RenderPagesForPrint failed";
    DidFinishPrinting(FAIL_PRINT);
  }
}
