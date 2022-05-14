 int PrintPreviewUI::GetAvailableDraftPageCount() {
  return print_preview_data_service()->GetAvailableDraftPageCount(
      preview_ui_addr_str_);
 }
