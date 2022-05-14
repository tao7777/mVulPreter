 void MockPrinter::UpdateSettings(int cookie,
                                  PrintMsg_PrintPages_Params* params,
                                  const std::vector<int>& pages) {
  if (document_cookie_ == -1) {
    document_cookie_ = CreateDocumentCookie();
  }
   params->Reset();
   params->pages = pages;
   SetPrintParams(&(params->params));
  printer_status_ = PRINTER_PRINTING;
}
