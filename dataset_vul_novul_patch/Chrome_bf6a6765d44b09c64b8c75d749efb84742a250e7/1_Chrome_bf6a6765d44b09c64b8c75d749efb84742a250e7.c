int PDFiumEngine::GetMostVisiblePage() {
   if (in_flight_visible_page_)
     return *in_flight_visible_page_;
 
   CalculateVisiblePages();
   return most_visible_page_;
 }
