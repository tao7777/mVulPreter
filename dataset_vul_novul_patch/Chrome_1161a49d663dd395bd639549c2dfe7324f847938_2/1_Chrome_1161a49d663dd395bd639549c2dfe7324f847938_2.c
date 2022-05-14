void TabStrip::OnDragEntered(const DropTargetEvent& event) {
  StopAnimating(true);

  UpdateDropIndex(event);

  GURL url;
   base::string16 title;
 
  if (event.data().GetURLAndTitle(&url, &title) && url.is_valid()) {
     drop_info_->url = url;
 
    if (url.SchemeIsFile())
      controller()->CheckFileSupported(url);
  }
}
