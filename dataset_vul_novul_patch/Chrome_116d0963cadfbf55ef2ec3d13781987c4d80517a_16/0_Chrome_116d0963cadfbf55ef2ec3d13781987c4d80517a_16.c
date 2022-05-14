 void PrintPreviewUI::OnCancelPendingPreviewRequest() {
  g_print_preview_request_id_map.Get().Set(id_, -1);
 }
