bool NavigationControllerImpl::RendererDidNavigate(
    RenderFrameHostImpl* rfh,
    const FrameHostMsg_DidCommitProvisionalLoad_Params& params,
    LoadCommittedDetails* details,
    bool is_navigation_within_page,
    NavigationHandleImpl* navigation_handle) {
  is_initial_navigation_ = false;

  bool overriding_user_agent_changed = false;
  if (GetLastCommittedEntry()) {
    details->previous_url = GetLastCommittedEntry()->GetURL();
    details->previous_entry_index = GetLastCommittedEntryIndex();
    if (pending_entry_ &&
        pending_entry_->GetIsOverridingUserAgent() !=
            GetLastCommittedEntry()->GetIsOverridingUserAgent())
      overriding_user_agent_changed = true;
  } else {
    details->previous_url = GURL();
    details->previous_entry_index = -1;
  }

  bool was_restored = false;
  DCHECK(pending_entry_index_ == -1 || pending_entry_->site_instance() ||
         pending_entry_->restore_type() != RestoreType::NONE);
  if (pending_entry_ && pending_entry_->restore_type() != RestoreType::NONE) {
    pending_entry_->set_restore_type(RestoreType::NONE);
    was_restored = true;
  }

  details->did_replace_entry = params.should_replace_current_entry;

  details->type = ClassifyNavigation(rfh, params);

  details->is_same_document = is_navigation_within_page;

  if (PendingEntryMatchesHandle(navigation_handle)) {
    if (pending_entry_->reload_type() != ReloadType::NONE) {
      last_committed_reload_type_ = pending_entry_->reload_type();
      last_committed_reload_time_ =
          time_smoother_.GetSmoothedTime(get_timestamp_callback_.Run());
    } else if (!pending_entry_->is_renderer_initiated() ||
               params.gesture == NavigationGestureUser) {
      last_committed_reload_type_ = ReloadType::NONE;
      last_committed_reload_time_ = base::Time();
    }
  }

  switch (details->type) {
    case NAVIGATION_TYPE_NEW_PAGE:
      RendererDidNavigateToNewPage(rfh, params, details->is_same_document,
                                   details->did_replace_entry,
                                   navigation_handle);
      break;
    case NAVIGATION_TYPE_EXISTING_PAGE:
      details->did_replace_entry = details->is_same_document;
      RendererDidNavigateToExistingPage(rfh, params, details->is_same_document,
                                        was_restored, navigation_handle);
      break;
    case NAVIGATION_TYPE_SAME_PAGE:
      RendererDidNavigateToSamePage(rfh, params, navigation_handle);
      break;
    case NAVIGATION_TYPE_NEW_SUBFRAME:
      RendererDidNavigateNewSubframe(rfh, params, details->is_same_document,
                                     details->did_replace_entry);
      break;
    case NAVIGATION_TYPE_AUTO_SUBFRAME:
      if (!RendererDidNavigateAutoSubframe(rfh, params)) {
        NotifyEntryChanged(GetLastCommittedEntry());
        return false;
      }
      break;
    case NAVIGATION_TYPE_NAV_IGNORE:
      if (pending_entry_) {
        DiscardNonCommittedEntries();
        delegate_->NotifyNavigationStateChanged(INVALIDATE_TYPE_URL);
      }
      return false;
    default:
      NOTREACHED();
  }

  base::Time timestamp =
      time_smoother_.GetSmoothedTime(get_timestamp_callback_.Run());
  DVLOG(1) << "Navigation finished at (smoothed) timestamp "
           << timestamp.ToInternalValue();

  DiscardNonCommittedEntriesInternal();

  DCHECK(params.page_state.IsValid()) << "Shouldn't see an empty PageState.";
  NavigationEntryImpl* active_entry = GetLastCommittedEntry();
   active_entry->SetTimestamp(timestamp);
   active_entry->SetHttpStatusCode(params.http_status_code);
 
  // Grab the corresponding FrameNavigationEntry for a few updates, but only if
  // the SiteInstance matches (to avoid updating the wrong entry by mistake).
  // A mismatch can occur if the renderer lies or due to a unique name collision
  // after a race with an OOPIF (see https://crbug.com/616820).
   FrameNavigationEntry* frame_entry =
       active_entry->GetFrameEntry(rfh->frame_tree_node());
  if (frame_entry && frame_entry->site_instance() != rfh->GetSiteInstance())
    frame_entry = nullptr;
  if (frame_entry) {
    frame_entry->SetPageState(params.page_state);
    frame_entry->set_redirect_chain(params.redirects);
  }

  if (!rfh->GetParent() &&
      IsBlockedNavigation(navigation_handle->GetNetErrorCode())) {
    DCHECK(params.url_is_unreachable);
    active_entry->SetURL(GURL(url::kAboutBlankURL));
    active_entry->SetVirtualURL(params.url);
    if (frame_entry) {
      frame_entry->SetPageState(
          PageState::CreateFromURL(active_entry->GetURL()));
    }
  }

  size_t redirect_chain_size = 0;
  for (size_t i = 0; i < params.redirects.size(); ++i) {
    redirect_chain_size += params.redirects[i].spec().length();
  }
  UMA_HISTOGRAM_COUNTS("Navigation.RedirectChainSize", redirect_chain_size);

  active_entry->ResetForCommit(frame_entry);

  if (!rfh->GetParent())
    CHECK_EQ(active_entry->site_instance(), rfh->GetSiteInstance());

  active_entry->SetBindings(rfh->GetEnabledBindings());

  details->entry = active_entry;
  details->is_main_frame = !rfh->GetParent();
  details->http_status_code = params.http_status_code;

  NotifyNavigationEntryCommitted(details);

  if (active_entry->GetURL().SchemeIs(url::kHttpsScheme) && !rfh->GetParent() &&
      navigation_handle->GetNetErrorCode() == net::OK) {
    UMA_HISTOGRAM_BOOLEAN("Navigation.SecureSchemeHasSSLStatus",
                          !!active_entry->GetSSL().certificate);
  }

  if (overriding_user_agent_changed)
    delegate_->UpdateOverridingUserAgent();

  int nav_entry_id = active_entry->GetUniqueID();
  for (FrameTreeNode* node : delegate_->GetFrameTree()->Nodes())
    node->current_frame_host()->set_nav_entry_id(nav_entry_id);
  return true;
}
