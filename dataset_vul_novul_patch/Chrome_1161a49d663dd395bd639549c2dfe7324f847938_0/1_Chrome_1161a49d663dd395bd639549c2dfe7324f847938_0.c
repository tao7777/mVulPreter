int BrowserRootView::OnPerformDrop(const ui::DropTargetEvent& event) {
  if (!forwarding_to_tab_strip_)
    return ui::DragDropTypes::DRAG_NONE;

   GURL url;
   base::string16 title;
   ui::OSExchangeData mapped_data;
  if (!event.data().GetURLAndTitle(&url, &title) || !url.is_valid()) {
     if (GetPasteAndGoURL(event.data(), &url))
       mapped_data.SetURL(url, base::string16());
  } else {
    mapped_data.SetURL(url, base::string16());
  }
  forwarding_to_tab_strip_ = false;
  scoped_ptr<ui::DropTargetEvent> mapped_event(
      MapEventToTabStrip(event, mapped_data));
  return tabstrip()->OnPerformDrop(*mapped_event);
}
