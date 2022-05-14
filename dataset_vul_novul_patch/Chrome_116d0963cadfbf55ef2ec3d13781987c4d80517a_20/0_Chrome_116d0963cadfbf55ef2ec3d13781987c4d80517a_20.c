 void PrintPreviewUI::OnReusePreviewData(int preview_request_id) {
  base::FundamentalValue ui_identifier(id_);
   base::FundamentalValue ui_preview_request_id(preview_request_id);
   web_ui()->CallJavascriptFunction("reloadPreviewPages", ui_identifier,
                                    ui_preview_request_id);
}
