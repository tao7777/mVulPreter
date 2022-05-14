void PrintPreviewUI::OnDidPreviewPage(int page_number,
                                       int preview_request_id) {
   DCHECK_GE(page_number, 0);
   base::FundamentalValue number(page_number);
  StringValue ui_identifier(preview_ui_addr_str_);
   base::FundamentalValue request_id(preview_request_id);
   web_ui()->CallJavascriptFunction(
       "onDidPreviewPage", number, ui_identifier, request_id);
 }
