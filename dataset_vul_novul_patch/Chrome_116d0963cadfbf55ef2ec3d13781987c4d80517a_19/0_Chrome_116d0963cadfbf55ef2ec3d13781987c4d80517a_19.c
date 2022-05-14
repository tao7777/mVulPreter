 void PrintPreviewUI::OnPrintPreviewRequest(int request_id) {
  g_print_preview_request_id_map.Get().Set(id_, request_id);
 }
