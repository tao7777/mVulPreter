bool PrintWebViewHelper::PrintPreviewContext::CreatePreviewDocument(
    PrintMsg_Print_Params* print_params,
    const std::vector<int>& pages) {
   DCHECK_EQ(INITIALIZED, state_);
   state_ = RENDERING;
 
   metafile_.reset(new printing::PreviewMetafile);
   if (!metafile_->Init()) {
     set_error(PREVIEW_ERROR_METAFILE_INIT_FAILED);
    LOG(ERROR) << "PreviewMetafile Init failed";
    return false;
  }

  prep_frame_view_.reset(new PrepareFrameAndViewForPrint(*print_params, frame(),
                                                         node()));
   UpdatePrintableSizeInPrintParameters(frame_, node_,
                                        prep_frame_view_.get(), print_params);
 
  print_params_.reset(new PrintMsg_Print_Params(*print_params));

   total_page_count_ = prep_frame_view_->GetExpectedPageCount();
   if (total_page_count_ == 0) {
     LOG(ERROR) << "CreatePreviewDocument got 0 page count";
    set_error(PREVIEW_ERROR_ZERO_PAGES);
    return false;
  }

  int selected_page_count = pages.size();
  current_page_index_ = 0;
  print_ready_metafile_page_count_ = selected_page_count;
  pages_to_render_ = pages;

  if (selected_page_count == 0) {
    print_ready_metafile_page_count_ = total_page_count_;
    for (int i = 0; i < total_page_count_; ++i)
      pages_to_render_.push_back(i);
  } else if (generate_draft_pages_) {
    int pages_index = 0;
    for (int i = 0; i < total_page_count_; ++i) {
      if (pages_index < selected_page_count && i == pages[pages_index]) {
        pages_index++;
        continue;
      }
      pages_to_render_.push_back(i);
    }
  }

  document_render_time_ = base::TimeDelta();
  begin_time_ = base::TimeTicks::Now();

  return true;
}
