 int PDFiumEngine::GetVisiblePageIndex(FPDF_PAGE page) {
  // Copy visible_pages_ since it can change as a result of loading the page in
  // GetPage(). See https://crbug.com/822091.
  std::vector<int> visible_pages_copy(visible_pages_);
  for (int page_index : visible_pages_copy) {
     if (pages_[page_index]->GetPage() == page)
       return page_index;
   }
  return -1;
}
