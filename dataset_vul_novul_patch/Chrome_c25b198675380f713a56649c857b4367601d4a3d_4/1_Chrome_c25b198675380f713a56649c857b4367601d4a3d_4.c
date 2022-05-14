void LockScreenMediaControlsView::SetArtwork(
    base::Optional<gfx::ImageSkia> img) {
  if (!img.has_value()) {
    session_artwork_->SetImage(nullptr);
     return;
   }
 
  session_artwork_->SetImageSize(ScaleSizeToFitView(
      img->size(), gfx::Size(kArtworkViewWidth, kArtworkViewHeight)));
   session_artwork_->SetImage(*img);
 }
