void NavigationRequest::OnStartChecksComplete(
    NavigationThrottle::ThrottleCheckResult result) {
  DCHECK(result.action() != NavigationThrottle::DEFER);
  DCHECK(result.action() != NavigationThrottle::BLOCK_RESPONSE);

  if (on_start_checks_complete_closure_)
    on_start_checks_complete_closure_.Run();
  if (result.action() == NavigationThrottle::CANCEL_AND_IGNORE ||
      result.action() == NavigationThrottle::CANCEL ||
      result.action() == NavigationThrottle::BLOCK_REQUEST ||
      result.action() == NavigationThrottle::BLOCK_REQUEST_AND_COLLAPSE) {
#if DCHECK_IS_ON()
    if (result.action() == NavigationThrottle::BLOCK_REQUEST) {
      DCHECK(result.net_error_code() == net::ERR_BLOCKED_BY_CLIENT ||
             result.net_error_code() == net::ERR_BLOCKED_BY_ADMINISTRATOR);
    }
    else if (result.action() == NavigationThrottle::CANCEL_AND_IGNORE) {
      DCHECK_EQ(result.net_error_code(), net::ERR_ABORTED);
    }
#endif

    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::BindOnce(&NavigationRequest::OnRequestFailedInternal,
                       weak_factory_.GetWeakPtr(), false,
                       result.net_error_code(), base::nullopt, true,
                       result.error_page_content()));

    return;
  }

  DCHECK_NE(AssociatedSiteInstanceType::NONE, associated_site_instance_type_);
  RenderFrameHostImpl* navigating_frame_host =
      associated_site_instance_type_ == AssociatedSiteInstanceType::SPECULATIVE
          ? frame_tree_node_->render_manager()->speculative_frame_host()
          : frame_tree_node_->current_frame_host();
  DCHECK(navigating_frame_host);

  navigation_handle_->SetExpectedProcess(navigating_frame_host->GetProcess());

  BrowserContext* browser_context =
      frame_tree_node_->navigator()->GetController()->GetBrowserContext();
  StoragePartition* partition = BrowserContext::GetStoragePartition(
      browser_context, navigating_frame_host->GetSiteInstance());
  DCHECK(partition);

  bool can_create_service_worker =
      (frame_tree_node_->pending_frame_policy().sandbox_flags &
       blink::WebSandboxFlags::kOrigin) != blink::WebSandboxFlags::kOrigin;
  request_params_.should_create_service_worker = can_create_service_worker;
  if (can_create_service_worker) {
    ServiceWorkerContextWrapper* service_worker_context =
        static_cast<ServiceWorkerContextWrapper*>(
            partition->GetServiceWorkerContext());
    navigation_handle_->InitServiceWorkerHandle(service_worker_context);
  }

  if (IsSchemeSupportedForAppCache(common_params_.url)) {
    if (navigating_frame_host->GetRenderViewHost()
            ->GetWebkitPreferences()
            .application_cache_enabled) {
      navigation_handle_->InitAppCacheHandle(
          static_cast<ChromeAppCacheService*>(partition->GetAppCacheService()));
    }
  }

  request_params_.navigation_timing.fetch_start = base::TimeTicks::Now();

  GURL base_url;
#if defined(OS_ANDROID)
  NavigationEntry* last_committed_entry =
      frame_tree_node_->navigator()->GetController()->GetLastCommittedEntry();
  if (last_committed_entry)
    base_url = last_committed_entry->GetBaseURLForDataURL();
#endif
  const GURL& top_document_url =
       !base_url.is_empty()
           ? base_url
           : frame_tree_node_->frame_tree()->root()->current_url();

  // Walk the ancestor chain to determine whether all frames are same-site. If
  // not, the |site_for_cookies| is set to an empty URL.
  //
  // 'Document::SiteForCookies()' in Blink, which special-cases extension
  // URLs and a few other sharp edges.
  const FrameTreeNode* current = frame_tree_node_->parent();
  bool ancestors_are_same_site = true;
  while (current && ancestors_are_same_site) {
    if (!net::registry_controlled_domains::SameDomainOrHost(
            top_document_url, current->current_url(),
            net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
      ancestors_are_same_site = false;
    }
    current = current->parent();
  }
   const GURL& site_for_cookies =
      ancestors_are_same_site
          ? (frame_tree_node_->IsMainFrame() ? common_params_.url
                                             : top_document_url)
          : GURL::EmptyGURL();
   bool parent_is_main_frame = !frame_tree_node_->parent()
                                   ? false
                                   : frame_tree_node_->parent()->IsMainFrame();

  std::unique_ptr<NavigationUIData> navigation_ui_data;
  if (navigation_handle_->GetNavigationUIData())
    navigation_ui_data = navigation_handle_->GetNavigationUIData()->Clone();

  bool is_for_guests_only =
      navigation_handle_->GetStartingSiteInstance()->GetSiteURL().
          SchemeIs(kGuestScheme);

  bool report_raw_headers = false;
  RenderFrameDevToolsAgentHost::ApplyOverrides(
      frame_tree_node_, begin_params_.get(), &report_raw_headers);
  RenderFrameDevToolsAgentHost::OnNavigationRequestWillBeSent(*this);

  loader_ = NavigationURLLoader::Create(
      browser_context->GetResourceContext(), partition,
      std::make_unique<NavigationRequestInfo>(
          common_params_, begin_params_.Clone(), site_for_cookies,
          frame_tree_node_->IsMainFrame(), parent_is_main_frame,
          IsSecureFrame(frame_tree_node_->parent()),
          frame_tree_node_->frame_tree_node_id(), is_for_guests_only,
          report_raw_headers,
          navigating_frame_host->GetVisibilityState() ==
              blink::mojom::PageVisibilityState::kPrerender,
          blob_url_loader_factory_ ? blob_url_loader_factory_->Clone()
                                   : nullptr),
      std::move(navigation_ui_data),
      navigation_handle_->service_worker_handle(),
      navigation_handle_->appcache_handle(), this);
}
