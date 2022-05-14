void PrintPreviewUI::OnDidPreviewPage(int page_number,
                                       int preview_request_id) {
   DCHECK_GE(page_number, 0);
   base::FundamentalValue number(page_number);
  base::FundamentalValue ui_identifier(id_);
   base::FundamentalValue request_id(preview_request_id);
   web_ui()->CallJavascriptFunction(
       "onDidPreviewPage", number, ui_identifier, request_id);
 }
