 void MockPrinter::UpdateSettings(int cookie,
                                  PrintMsg_PrintPages_Params* params,
                                  const std::vector<int>& pages) {
  EXPECT_EQ(document_cookie_, cookie);
   params->Reset();
   params->pages = pages;
   SetPrintParams(&(params->params));
  printer_status_ = PRINTER_PRINTING;
}
