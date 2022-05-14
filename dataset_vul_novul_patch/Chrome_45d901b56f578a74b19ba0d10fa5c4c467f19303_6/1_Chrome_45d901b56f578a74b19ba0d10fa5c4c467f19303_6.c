void GM2TabStyle::PaintTab(gfx::Canvas* canvas, const SkPath& clip) const {
  int active_tab_fill_id = 0;
  int active_tab_y_inset = 0;
  if (tab_->GetThemeProvider()->HasCustomImage(IDR_THEME_TOOLBAR)) {
    active_tab_fill_id = IDR_THEME_TOOLBAR;
    active_tab_y_inset = GetStrokeThickness(true);
   }
 
   if (tab_->IsActive()) {
    PaintTabBackground(canvas, true /* active */, active_tab_fill_id,
                        active_tab_y_inset, nullptr /* clip */);
   } else {
     PaintInactiveTabBackground(canvas, clip);

    const float throb_value = GetThrobValue();
     if (throb_value > 0) {
       canvas->SaveLayerAlpha(gfx::ToRoundedInt(throb_value * 0xff),
                              tab_->GetLocalBounds());
      PaintTabBackground(canvas, true /* active */, active_tab_fill_id,
                          active_tab_y_inset, nullptr /* clip */);
       canvas->Restore();
     }
  }
}
