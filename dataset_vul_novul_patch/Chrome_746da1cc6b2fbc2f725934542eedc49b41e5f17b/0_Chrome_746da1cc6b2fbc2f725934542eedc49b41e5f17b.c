 void PrintViewManager::PrintPreviewDone() {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (print_preview_state_ == NOT_PREVIEWING)
    return;
 
   if (print_preview_state_ == SCRIPTED_PREVIEW) {
     auto& map = g_scripted_print_preview_closure_map.Get();
    auto it = map.find(scripted_print_preview_rph_);
    CHECK(it != map.end());
    it->second.Run();
    map.erase(it);
    scripted_print_preview_rph_ = nullptr;
  }
  print_preview_state_ = NOT_PREVIEWING;
  print_preview_rfh_ = nullptr;
}
