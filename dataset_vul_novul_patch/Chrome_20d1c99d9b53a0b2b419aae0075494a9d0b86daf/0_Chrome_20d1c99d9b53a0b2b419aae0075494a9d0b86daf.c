bool NavigationController::RendererDidNavigate(
    const ViewHostMsg_FrameNavigate_Params& params,
    int extra_invalidate_flags,
    LoadCommittedDetails* details) {

  if (GetLastCommittedEntry()) {
    details->previous_url = GetLastCommittedEntry()->url();
    details->previous_entry_index = last_committed_entry_index();
  } else {
    details->previous_url = GURL();
    details->previous_entry_index = -1;
  }

  if (pending_entry_index_ >= 0 && !pending_entry_->site_instance()) {
    DCHECK(pending_entry_->restore_type() != NavigationEntry::RESTORE_NONE);
    pending_entry_->set_site_instance(tab_contents_->GetSiteInstance());
    pending_entry_->set_restore_type(NavigationEntry::RESTORE_NONE);
  }

  details->is_in_page = IsURLInPageNavigation(params.url);

  details->type = ClassifyNavigation(params);

  switch (details->type) {
    case NavigationType::NEW_PAGE:
      RendererDidNavigateToNewPage(params, &(details->did_replace_entry));
      break;
    case NavigationType::EXISTING_PAGE:
      RendererDidNavigateToExistingPage(params);
      break;
    case NavigationType::SAME_PAGE:
      RendererDidNavigateToSamePage(params);
      break;
    case NavigationType::IN_PAGE:
      RendererDidNavigateInPage(params, &(details->did_replace_entry));
      break;
    case NavigationType::NEW_SUBFRAME:
      RendererDidNavigateNewSubframe(params);
      break;
    case NavigationType::AUTO_SUBFRAME:
      if (!RendererDidNavigateAutoSubframe(params))
         return false;
       break;
     case NavigationType::NAV_IGNORE:
      // If a pending navigation was in progress, this canceled it.  We should
      // discard it and make sure it is removed from the URL bar.  After that,
      // there is nothing we can do with this navigation, so we just return to
      if (pending_entry_) {
        DiscardNonCommittedEntries();
        extra_invalidate_flags |= TabContents::INVALIDATE_URL;
        tab_contents_->NotifyNavigationStateChanged(extra_invalidate_flags);
      }
       return false;
     default:
       NOTREACHED();
  }

  DCHECK(!params.content_state.empty());
  NavigationEntry* active_entry = GetActiveEntry();
  active_entry->set_content_state(params.content_state);

  DCHECK(active_entry->site_instance() == tab_contents_->GetSiteInstance());

  details->is_auto = (PageTransition::IsRedirect(params.transition) &&
                      !pending_entry()) ||
      params.gesture == NavigationGestureAuto;

  details->entry = active_entry;
  details->is_main_frame = PageTransition::IsMainFrame(params.transition);
  details->serialized_security_info = params.security_info;
  details->http_status_code = params.http_status_code;
  NotifyNavigationEntryCommitted(details, extra_invalidate_flags);

  return true;
}
