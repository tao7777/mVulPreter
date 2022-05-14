 void PrintWebViewHelper::OnPrintPages() {
  CHECK_LE(ipc_nesting_level_, 1);
   blink::WebLocalFrame* frame;
   if (!GetPrintFrame(&frame))
     return;
  auto plugin = delegate_->GetPdfElement(frame);
  Print(frame, plugin, false);
 }
