 void PrintPreviewUI::SetPrintPreviewDataForIndex(
     int index,
     const base::RefCountedBytes* data) {
  print_preview_data_service()->SetDataEntry(preview_ui_addr_str_, index, data);
 }
