 void PrintPreviewUI::GetPrintPreviewDataForIndex(
     int index,
     scoped_refptr<base::RefCountedBytes>* data) {
  print_preview_data_service()->GetDataEntry(id_, index, data);
 }
