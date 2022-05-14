 void PrintWebViewHelper::OnInitiatePrintPreview(bool selection_only) {
  CHECK_LE(ipc_nesting_level_, 1);
   blink::WebLocalFrame* frame = NULL;
   GetPrintFrame(&frame);
   DCHECK(frame);
  auto plugin = delegate_->GetPdfElement(frame);
  if (!plugin.isNull()) {
    PrintNode(plugin);
    return;
  }
  print_preview_context_.InitWithFrame(frame);
  RequestPrintPreview(selection_only
                          ? PRINT_PREVIEW_USER_INITIATED_SELECTION
                          : PRINT_PREVIEW_USER_INITIATED_ENTIRE_FRAME);
}
