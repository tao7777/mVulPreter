 NativeBrowserFrame* NativeBrowserFrame::CreateNativeBrowserFrame(
     BrowserFrame* browser_frame,
     BrowserView* browser_view) {
  if (views::Widget::IsPureViews())
     return new BrowserFrameViews(browser_frame, browser_view);
   return new BrowserFrameGtk(browser_frame, browser_view);
 }
