int PDFiumEngine::GetMostVisiblePage() {
   if (in_flight_visible_page_)
     return *in_flight_visible_page_;
 
  // We can call GetMostVisiblePage through a callback from PDFium. We have
  // to defer the page deletion otherwise we could potentially delete the page
  // that originated the calling JS request and destroy the objects that are
  // currently being used.
  defer_page_unload_ = true;
   CalculateVisiblePages();
  defer_page_unload_ = false;
   return most_visible_page_;
 }
