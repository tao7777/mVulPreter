 void PrintPreviewUI::SetPrintPreviewDataForIndex(
     int index,
     const base::RefCountedBytes* data) {
  print_preview_data_service()->SetDataEntry(id_, index, data);
 }
