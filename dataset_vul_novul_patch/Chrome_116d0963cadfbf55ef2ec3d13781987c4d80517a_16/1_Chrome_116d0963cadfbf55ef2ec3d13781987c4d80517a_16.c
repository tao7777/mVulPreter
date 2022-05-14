 void PrintPreviewUI::OnCancelPendingPreviewRequest() {
  g_print_preview_request_id_map.Get().Set(preview_ui_addr_str_, -1);
 }
