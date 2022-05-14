 bool MediaControlsProgressView::OnMousePressed(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton() || event.y() < kMinClickHeight ||
      event.y() > kMaxClickHeight) {
     return false;
   }
 
  HandleSeeking(event.location());
   return true;
 }
