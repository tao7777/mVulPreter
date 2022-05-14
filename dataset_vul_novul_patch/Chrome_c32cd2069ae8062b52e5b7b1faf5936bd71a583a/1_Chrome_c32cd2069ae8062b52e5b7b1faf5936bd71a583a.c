void NavigationControllerImpl::RendererDidNavigateToExistingPage(
    RenderFrameHostImpl* rfh,
    const FrameHostMsg_DidCommitProvisionalLoad_Params& params,
    bool is_in_page,
    bool was_restored,
    NavigationHandleImpl* handle) {
  DCHECK(!rfh->GetParent());


  NavigationEntryImpl* entry;
  if (params.intended_as_new_entry) {
     entry = GetLastCommittedEntry();
   } else if (params.nav_entry_id) {
     entry = GetEntryWithUniqueID(params.nav_entry_id);

    if (is_in_page) {
      NavigationEntryImpl* last_entry = GetLastCommittedEntry();
      if (entry->GetURL().GetOrigin() == last_entry->GetURL().GetOrigin() &&
          last_entry->GetSSL().initialized && !entry->GetSSL().initialized &&
          was_restored) {
        entry->GetSSL() = last_entry->GetSSL();
      }
    } else {
      entry->GetSSL() = handle->ssl_status();
    }
  } else {
    entry = GetLastCommittedEntry();

    if (!is_in_page)
      entry->GetSSL() = handle->ssl_status();
  }
  DCHECK(entry);

  entry->set_page_type(params.url_is_unreachable ? PAGE_TYPE_ERROR
                                                 : PAGE_TYPE_NORMAL);
  entry->SetURL(params.url);
  entry->SetReferrer(params.referrer);
  if (entry->update_virtual_url_with_url())
    UpdateVirtualURLToURL(entry, params.url);

  DCHECK(entry->site_instance() == nullptr ||
         !entry->GetRedirectChain().empty() ||
         entry->site_instance() == rfh->GetSiteInstance());

  entry->AddOrUpdateFrameEntry(
      rfh->frame_tree_node(), params.item_sequence_number,
      params.document_sequence_number, rfh->GetSiteInstance(), nullptr,
      params.url, params.referrer, params.redirects, params.page_state,
      params.method, params.post_id);

  if (ui::PageTransitionIsRedirect(params.transition) && !is_in_page)
    entry->GetFavicon() = FaviconStatus();

  DiscardNonCommittedEntriesInternal();

  last_committed_entry_index_ = GetIndexOfEntry(entry);
}
