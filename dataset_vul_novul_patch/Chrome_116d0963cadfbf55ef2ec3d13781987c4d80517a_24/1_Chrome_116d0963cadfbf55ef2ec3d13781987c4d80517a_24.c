 PrintPreviewUI::~PrintPreviewUI() {
  print_preview_data_service()->RemoveEntry(preview_ui_addr_str_);
  g_print_preview_request_id_map.Get().Erase(preview_ui_addr_str_);
 }
