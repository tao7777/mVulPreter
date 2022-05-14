std::string PrintPreviewUI::GetPrintPreviewUIAddress() const {
  char preview_ui_addr[2 + (2 * sizeof(this)) + 1];
  base::snprintf(preview_ui_addr, sizeof(preview_ui_addr), "%p", this);
  return preview_ui_addr;
 }
