void MediaControlsProgressView::HandleSeeking(
void MediaControlsProgressView::HandleSeeking(const gfx::Point& location) {
  gfx::Point location_in_bar(location);
  ConvertPointToTarget(this, progress_bar_, &location_in_bar);

   double seek_to_progress =
       static_cast<double>(location_in_bar.x()) / progress_bar_->width();
   seek_callback_.Run(seek_to_progress);
}
