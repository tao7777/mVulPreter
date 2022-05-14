RenderFrameHostManager::DetermineSiteInstanceForURL(
    const GURL& dest_url,
    SiteInstance* source_instance,
    SiteInstance* current_instance,
    SiteInstance* dest_instance,
    ui::PageTransition transition,
    bool dest_is_restore,
    bool dest_is_view_source_mode,
    bool force_browsing_instance_swap,
    bool was_server_redirect) {
  SiteInstanceImpl* current_instance_impl =
      static_cast<SiteInstanceImpl*>(current_instance);
  NavigationControllerImpl& controller =
      delegate_->GetControllerForRenderManager();
  BrowserContext* browser_context = controller.GetBrowserContext();

  if (dest_instance) {
    if (force_browsing_instance_swap) {
      CHECK(!dest_instance->IsRelatedSiteInstance(
                render_frame_host_->GetSiteInstance()));
    }
    return SiteInstanceDescriptor(dest_instance);
  }

  if (force_browsing_instance_swap)
    return SiteInstanceDescriptor(browser_context, dest_url,
                                  SiteInstanceRelation::UNRELATED);

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kProcessPerSite) &&
      ui::PageTransitionCoreTypeIs(transition, ui::PAGE_TRANSITION_GENERATED)) {
    return SiteInstanceDescriptor(current_instance_impl);
  }

  if (SiteIsolationPolicy::AreCrossProcessFramesPossible() &&
      !frame_tree_node_->IsMainFrame()) {
    SiteInstance* parent_site_instance =
        frame_tree_node_->parent()->current_frame_host()->GetSiteInstance();
    if (parent_site_instance->GetSiteURL().SchemeIs(kChromeUIScheme) &&
        dest_url.SchemeIs(kChromeUIScheme)) {
      return SiteInstanceDescriptor(parent_site_instance);
    }
  }

  if (!current_instance_impl->HasSite()) {
    bool use_process_per_site =
        RenderProcessHost::ShouldUseProcessPerSite(browser_context, dest_url) &&
        RenderProcessHostImpl::GetProcessHostForSite(browser_context, dest_url);
    if (current_instance_impl->HasRelatedSiteInstance(dest_url) ||
        use_process_per_site) {
      return SiteInstanceDescriptor(browser_context, dest_url,
                                    SiteInstanceRelation::RELATED);
    }

    if (current_instance_impl->HasWrongProcessForURL(dest_url))
      return SiteInstanceDescriptor(browser_context, dest_url,
                                    SiteInstanceRelation::RELATED);

    if (dest_is_view_source_mode)
      return SiteInstanceDescriptor(browser_context, dest_url,
                                    SiteInstanceRelation::UNRELATED);

    if (WebUIControllerFactoryRegistry::GetInstance()->UseWebUIForURL(
            browser_context, dest_url)) {
      return SiteInstanceDescriptor(browser_context, dest_url,
                                    SiteInstanceRelation::UNRELATED);
    }

    if (dest_is_restore &&
        GetContentClient()->browser()->ShouldAssignSiteForURL(dest_url)) {
      current_instance_impl->SetSite(dest_url);
    }

    return SiteInstanceDescriptor(current_instance_impl);
  }


   NavigationEntry* current_entry = controller.GetLastCommittedEntry();
  if (delegate_->GetInterstitialForRenderManager()) {
     current_entry = controller.GetEntryAtOffset(-1);
  }

  if (current_entry &&
      current_entry->IsViewSourceMode() != dest_is_view_source_mode &&
      !IsRendererDebugURL(dest_url)) {
    return SiteInstanceDescriptor(browser_context, dest_url,
                                  SiteInstanceRelation::UNRELATED);
  }

  GURL about_blank(url::kAboutBlankURL);
  GURL about_srcdoc(content::kAboutSrcDocURL);
  bool dest_is_data_or_about = dest_url == about_srcdoc ||
                               dest_url == about_blank ||
                               dest_url.scheme() == url::kDataScheme;
  if (source_instance && dest_is_data_or_about && !was_server_redirect)
    return SiteInstanceDescriptor(source_instance);

  if (IsCurrentlySameSite(render_frame_host_.get(), dest_url))
    return SiteInstanceDescriptor(render_frame_host_->GetSiteInstance());

  if (SiteIsolationPolicy::IsTopDocumentIsolationEnabled()) {
    if (!frame_tree_node_->IsMainFrame()) {
      RenderFrameHostImpl* main_frame =
          frame_tree_node_->frame_tree()->root()->current_frame_host();
      if (IsCurrentlySameSite(main_frame, dest_url))
        return SiteInstanceDescriptor(main_frame->GetSiteInstance());
    }

    if (frame_tree_node_->opener()) {
      RenderFrameHostImpl* opener_frame =
          frame_tree_node_->opener()->current_frame_host();
      if (IsCurrentlySameSite(opener_frame, dest_url))
        return SiteInstanceDescriptor(opener_frame->GetSiteInstance());
    }
  }

  if (!frame_tree_node_->IsMainFrame() &&
      SiteIsolationPolicy::IsTopDocumentIsolationEnabled() &&
      !SiteInstanceImpl::DoesSiteRequireDedicatedProcess(browser_context,
                                                         dest_url)) {
    if (GetContentClient()
            ->browser()
            ->ShouldFrameShareParentSiteInstanceDespiteTopDocumentIsolation(
                dest_url, current_instance)) {
      return SiteInstanceDescriptor(render_frame_host_->GetSiteInstance());
    }

    return SiteInstanceDescriptor(
        browser_context, dest_url,
        SiteInstanceRelation::RELATED_DEFAULT_SUBFRAME);
  }

  if (!frame_tree_node_->IsMainFrame()) {
    RenderFrameHostImpl* parent =
        frame_tree_node_->parent()->current_frame_host();
    bool dest_url_requires_dedicated_process =
        SiteInstanceImpl::DoesSiteRequireDedicatedProcess(browser_context,
                                                          dest_url);
    if (!parent->GetSiteInstance()->RequiresDedicatedProcess() &&
        !dest_url_requires_dedicated_process) {
      return SiteInstanceDescriptor(parent->GetSiteInstance());
    }
  }

  return SiteInstanceDescriptor(browser_context, dest_url,
                                SiteInstanceRelation::RELATED);
}
