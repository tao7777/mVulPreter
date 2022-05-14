int TabStrip::OnPerformDrop(const DropTargetEvent& event) {
  if (!drop_info_.get())
    return ui::DragDropTypes::DRAG_NONE;

  const int drop_index = drop_info_->drop_index;
  const bool drop_before = drop_info_->drop_before;
  const bool file_supported = drop_info_->file_supported;

  SetDropIndex(-1, false);

   GURL url;
   base::string16 title;
   if (!file_supported ||
      !event.data().GetURLAndTitle(
           ui::OSExchangeData::CONVERT_FILENAMES, &url, &title) ||
      !url.is_valid())
     return ui::DragDropTypes::DRAG_NONE;
 
   controller()->PerformDrop(drop_before, drop_index, url);

  return GetDropEffect(event);
}
