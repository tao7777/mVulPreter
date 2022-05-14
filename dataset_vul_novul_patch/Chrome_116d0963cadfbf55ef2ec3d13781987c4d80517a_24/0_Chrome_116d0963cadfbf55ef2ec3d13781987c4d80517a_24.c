 PrintPreviewUI::~PrintPreviewUI() {
  print_preview_data_service()->RemoveEntry(id_);
  g_print_preview_request_id_map.Get().Erase(id_);
  g_print_preview_ui_id_map.Get().Remove(id_);
 }
