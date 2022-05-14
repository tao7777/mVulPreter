 void PrintWebViewHelper::OnPrintPages() {
   blink::WebLocalFrame* frame;
   if (!GetPrintFrame(&frame))
     return;
  auto plugin = delegate_->GetPdfElement(frame);
  Print(frame, plugin, false);
 }
