void WmOverviewSnapshot::Init(const gfx::Size& size,
                              Browser* browser,
                              int index) {
  snapshot_view_ = new views::ImageView();
  MakeTransparent();

  snapshot_view_->set_background(
      views::Background::CreateSolidBackground(SK_ColorWHITE));
   snapshot_view_->set_border(
       views::Border::CreateSolidBorder(1, SkColorSetRGB(176, 176, 176)));
 
  WidgetGtk::Init(NULL, gfx::Rect(gfx::Point(0,0), size));
 
   SetContentsView(snapshot_view_);
 
  UpdateIndex(browser, index);
}
