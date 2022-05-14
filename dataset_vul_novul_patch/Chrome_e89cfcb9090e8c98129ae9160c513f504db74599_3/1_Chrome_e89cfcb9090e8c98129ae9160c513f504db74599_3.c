void Browser::TabDetachedAtImpl(TabContents* contents, int index,
                                 DetachType type) {
   if (type == DETACH_TYPE_DETACH) {
    if (contents == chrome::GetActiveTabContents(this)) {
       LocationBar* location_bar = window()->GetLocationBar();
       if (location_bar)
        location_bar->SaveStateToContents(contents->web_contents());
     }
 
     if (!tab_strip_model_->closing_all())
       SyncHistoryWithTabs(0);
   }
 
  SetAsDelegate(contents->web_contents(), NULL);
  RemoveScheduledUpdatesFor(contents->web_contents());
 
   if (find_bar_controller_.get() && index == active_index()) {
     find_bar_controller_->ChangeWebContents(NULL);
   }
 
  search_delegate_->OnTabDetached(contents->web_contents());
 
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_ATTACHED,
                    content::Source<WebContents>(contents->web_contents()));
   registrar_.Remove(this, content::NOTIFICATION_INTERSTITIAL_DETACHED,
                    content::Source<WebContents>(contents->web_contents()));
 }
