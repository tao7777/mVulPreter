void Browser::TabDetachedAtImpl(TabContents* contents, int index,
void Browser::TabDetachedAtImpl(content::WebContents* contents,
                                int index,
                                 DetachType type) {
   if (type == DETACH_TYPE_DETACH) {
    if (contents == chrome::GetActiveWebContents(this)) {
       LocationBar* location_bar = window()->GetLocationBar();
       if (location_bar)
        location_bar->SaveStateToContents(contents);
     }
 
     if (!tab_strip_model_->closing_all())
       SyncHistoryWithTabs(0);
   }
 
  SetAsDelegate(contents, NULL);
  RemoveScheduledUpdatesFor(contents);
 
   if (find_bar_controller_.get() && index == active_index()) {
     find_bar_controller_->ChangeWebContents(NULL);
   }
 
  search_delegate_->OnTabDetached(contents);
 
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_ATTACHED,
                    content::Source<WebContents>(contents));
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_DETACHED,
                    content::Source<WebContents>(contents));
 }
