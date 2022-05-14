void PrintWebViewHelper::PrintNode(const WebKit::WebNode& node) {
  if (node.isNull() || !node.document().frame()) {
     return;
   }
 
  if (print_node_in_progress_) {
    // This can happen as a result of processing sync messages when printing
    // from ppapi plugins. It's a rare case, so its OK to just fail here.
    // See http://crbug.com/159165.
    return;
  }

  print_node_in_progress_ = true;

   if (is_preview_enabled_) {
    print_preview_context_.InitWithNode(node);
    RequestPrintPreview(PRINT_PREVIEW_USER_INITIATED_CONTEXT_NODE);
  } else {
     WebKit::WebNode duplicate_node(node);
     Print(duplicate_node.document().frame(), duplicate_node);
   }

  print_node_in_progress_ = false;
 }
