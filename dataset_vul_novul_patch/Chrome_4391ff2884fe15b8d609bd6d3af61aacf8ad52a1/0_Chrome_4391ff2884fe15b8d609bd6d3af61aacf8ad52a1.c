void NavigationControllerImpl::Reload(ReloadType reload_type,
                                      bool check_for_repost) {
  DCHECK_NE(ReloadType::NONE, reload_type);

  if (transient_entry_index_ != -1) {
    NavigationEntryImpl* transient_entry = GetTransientEntry();
    if (!transient_entry)
      return;
    LoadURL(transient_entry->GetURL(),
            Referrer(),
            ui::PAGE_TRANSITION_RELOAD,
            transient_entry->extra_headers());
    return;
  }

  NavigationEntryImpl* entry = nullptr;
  int current_index = -1;

  if (IsInitialNavigation() && pending_entry_) {
    entry = pending_entry_;
    current_index = pending_entry_index_;
  } else {
    DiscardNonCommittedEntriesInternal();
    current_index = GetCurrentEntryIndex();
    if (current_index != -1) {
      entry = GetEntryAtIndex(current_index);
    }
  }

  if (!entry)
    return;

  if (last_committed_reload_type_ != ReloadType::NONE) {
    DCHECK(!last_committed_reload_time_.is_null());
    base::Time now =
        time_smoother_.GetSmoothedTime(get_timestamp_callback_.Run());
    DCHECK_GT(now, last_committed_reload_time_);
    if (!last_committed_reload_time_.is_null() &&
        now > last_committed_reload_time_) {
      base::TimeDelta delta = now - last_committed_reload_time_;
      UMA_HISTOGRAM_MEDIUM_TIMES("Navigation.Reload.ReloadToReloadDuration",
                                 delta);
      if (last_committed_reload_type_ == ReloadType::NORMAL) {
        UMA_HISTOGRAM_MEDIUM_TIMES(
            "Navigation.Reload.ReloadMainResourceToReloadDuration", delta);
      }
    }
  }

  entry->set_reload_type(reload_type);

  if (g_check_for_repost && check_for_repost &&
      entry->GetHasPostData()) {
    delegate_->NotifyBeforeFormRepostWarningShow();

    pending_reload_ = reload_type;
    delegate_->ActivateAndShowRepostFormWarningDialog();
  } else {
    if (!IsInitialNavigation())
      DiscardNonCommittedEntriesInternal();

    SiteInstanceImpl* site_instance = entry->site_instance();
    bool is_for_guests_only = site_instance && site_instance->HasProcess() &&
        site_instance->GetProcess()->IsForGuestsOnly();
    if (!is_for_guests_only && site_instance &&
        site_instance->HasWrongProcessForURL(entry->GetURL())) {
      NavigationEntryImpl* nav_entry = NavigationEntryImpl::FromNavigationEntry(
          CreateNavigationEntry(entry->GetURL(), entry->GetReferrer(),
                                entry->GetTransitionType(), false,
                                entry->extra_headers(), browser_context_,
                                nullptr /* blob_url_loader_factory */)
              .release());

       reload_type = ReloadType::NONE;
 
       nav_entry->set_should_replace_entry(true);
      nav_entry->set_is_renderer_initiated(entry->is_renderer_initiated());
       pending_entry_ = nav_entry;
       DCHECK_EQ(-1, pending_entry_index_);
     } else {
      pending_entry_ = entry;
      pending_entry_index_ = current_index;

      pending_entry_->SetTransitionType(ui::PAGE_TRANSITION_RELOAD);
    }

    NavigateToPendingEntry(reload_type, nullptr /* navigation_ui_data */);
  }
}
