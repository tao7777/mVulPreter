void PrintPreviewUI::GetCurrentPrintPreviewStatus(
void PrintPreviewUI::GetCurrentPrintPreviewStatus(int32 preview_ui_id,
                                                  int request_id,
                                                  bool* cancel) {
   int current_id = -1;
  if (!g_print_preview_request_id_map.Get().Get(preview_ui_id, &current_id)) {
     *cancel = true;
     return;
   }
   *cancel = (request_id != current_id);
 }
