void Browser::AddNewContents(WebContents* source,
                             std::unique_ptr<WebContents> new_contents,
                             WindowOpenDisposition disposition,
                              const gfx::Rect& initial_rect,
                              bool user_gesture,
                              bool* was_blocked) {
   if (source && PopupBlockerTabHelper::ConsiderForPopupBlocking(disposition))
    PopupTracker::CreateForWebContents(new_contents.get(), source);
  chrome::AddWebContents(this, source, std::move(new_contents), disposition,
                         initial_rect);
}
