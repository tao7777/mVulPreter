void PrintPreviewUI::OnPreviewDataIsAvailable(int expected_pages_count,
                                              int preview_request_id) {
  VLOG(1) << "Print preview request finished with "
          << expected_pages_count << " pages";

  if (!initial_preview_start_time_.is_null()) {
    UMA_HISTOGRAM_TIMES("PrintPreview.InitialDisplayTime",
                        base::TimeTicks::Now() - initial_preview_start_time_);
    UMA_HISTOGRAM_COUNTS("PrintPreview.PageCount.Initial",
                          expected_pages_count);
     initial_preview_start_time_ = base::TimeTicks();
   }
  base::StringValue ui_identifier(preview_ui_addr_str_);
   base::FundamentalValue ui_preview_request_id(preview_request_id);
   web_ui()->CallJavascriptFunction("updatePrintPreview", ui_identifier,
                                    ui_preview_request_id);
}
