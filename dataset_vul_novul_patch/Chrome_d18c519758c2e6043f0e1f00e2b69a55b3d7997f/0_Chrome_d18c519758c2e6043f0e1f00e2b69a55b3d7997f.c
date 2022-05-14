void WebContentsImpl::CreateNewWindow(
    RenderFrameHost* opener,
    int32_t render_view_route_id,
    int32_t main_frame_route_id,
    int32_t main_frame_widget_route_id,
    const mojom::CreateNewWindowParams& params,
    SessionStorageNamespace* session_storage_namespace) {
  DCHECK_EQ((render_view_route_id == MSG_ROUTING_NONE),
            (main_frame_route_id == MSG_ROUTING_NONE));
  DCHECK_EQ((render_view_route_id == MSG_ROUTING_NONE),
            (main_frame_widget_route_id == MSG_ROUTING_NONE));
  DCHECK(opener);

  int render_process_id = opener->GetProcess()->GetID();
  SiteInstance* source_site_instance = opener->GetSiteInstance();

  DCHECK(!RenderFrameHostImpl::FromID(render_process_id, main_frame_route_id));

  bool is_guest = BrowserPluginGuest::IsGuest(this);

  DCHECK(!params.opener_suppressed || render_view_route_id == MSG_ROUTING_NONE);

  scoped_refptr<SiteInstance> site_instance =
      params.opener_suppressed && !is_guest
          ? SiteInstance::CreateForURL(GetBrowserContext(), params.target_url)
          : source_site_instance;

  const std::string& partition_id =
      GetContentClient()->browser()->
          GetStoragePartitionIdForSite(GetBrowserContext(),
                                       site_instance->GetSiteURL());
  StoragePartition* partition = BrowserContext::GetStoragePartition(
      GetBrowserContext(), site_instance.get());
  DOMStorageContextWrapper* dom_storage_context =
      static_cast<DOMStorageContextWrapper*>(partition->GetDOMStorageContext());
  SessionStorageNamespaceImpl* session_storage_namespace_impl =
      static_cast<SessionStorageNamespaceImpl*>(session_storage_namespace);
  CHECK(session_storage_namespace_impl->IsFromContext(dom_storage_context));

  if (delegate_ &&
      !delegate_->ShouldCreateWebContents(
          this, opener, source_site_instance, render_view_route_id,
          main_frame_route_id, main_frame_widget_route_id,
          params.window_container_type, opener->GetLastCommittedURL(),
          params.frame_name, params.target_url, partition_id,
          session_storage_namespace)) {
    RenderFrameHostImpl* rfh =
        RenderFrameHostImpl::FromID(render_process_id, main_frame_route_id);
    if (rfh) {
      DCHECK(rfh->IsRenderFrameLive());
      rfh->Init();
    }
    return;
  }

  CreateParams create_params(GetBrowserContext(), site_instance.get());
  create_params.routing_id = render_view_route_id;
  create_params.main_frame_routing_id = main_frame_route_id;
  create_params.main_frame_widget_routing_id = main_frame_widget_route_id;
  create_params.main_frame_name = params.frame_name;
  create_params.opener_render_process_id = render_process_id;
  create_params.opener_render_frame_id = opener->GetRoutingID();
  create_params.opener_suppressed = params.opener_suppressed;
  if (params.disposition == WindowOpenDisposition::NEW_BACKGROUND_TAB)
    create_params.initially_hidden = true;
  create_params.renderer_initiated_creation =
      main_frame_route_id != MSG_ROUTING_NONE;

  std::unique_ptr<WebContents> new_contents;
  if (!is_guest) {
    create_params.context = view_->GetNativeView();
    create_params.initial_size = GetContainerBounds().size();
    new_contents = WebContents::Create(create_params);
  }  else {
    new_contents = base::WrapUnique(
        GetBrowserPluginGuest()->CreateNewGuestWindow(create_params));
  }
  WebContentsImpl* raw_new_contents =
      static_cast<WebContentsImpl*>(new_contents.get());
  raw_new_contents->GetController().SetSessionStorageNamespace(
      partition_id, session_storage_namespace);

  if (!params.frame_name.empty())
    raw_new_contents->GetRenderManager()->CreateProxiesForNewNamedFrame();

  if (!params.opener_suppressed) {
    if (!is_guest) {
      WebContentsView* new_view = raw_new_contents->view_.get();

      new_view->CreateViewForWidget(
          new_contents->GetRenderViewHost()->GetWidget(), false);
    }
    DCHECK_NE(MSG_ROUTING_NONE, main_frame_widget_route_id);
    pending_contents_[std::make_pair(render_process_id,
                                     main_frame_widget_route_id)] =
        std::move(new_contents);
    AddDestructionObserver(raw_new_contents);
  }

  if (delegate_) {
    delegate_->WebContentsCreated(this, render_process_id,
                                  opener->GetRoutingID(), params.frame_name,
                                  params.target_url, raw_new_contents);
  }

  if (opener) {
    for (auto& observer : observers_) {
      observer.DidOpenRequestedURL(raw_new_contents, opener, params.target_url,
                                   params.referrer, params.disposition,
                                   ui::PAGE_TRANSITION_LINK,
                                   false,  // started_from_context_menu
                                   true);  // renderer_initiated
    }
  }
 
  ForSecurityDropFullscreen();
 
   if (params.opener_suppressed) {
    bool was_blocked = false;

    base::WeakPtr<WebContentsImpl> weak_new_contents =
        raw_new_contents->weak_factory_.GetWeakPtr();
    if (delegate_) {
      gfx::Rect initial_rect;

      delegate_->AddNewContents(this, std::move(new_contents),
                                params.disposition, initial_rect,
                                params.mimic_user_gesture, &was_blocked);
      if (!weak_new_contents)
        return;  // The delegate deleted |new_contents| during AddNewContents().
    }

    if (!was_blocked) {
      OpenURLParams open_params(params.target_url, params.referrer,
                                WindowOpenDisposition::CURRENT_TAB,
                                ui::PAGE_TRANSITION_LINK,
                                true /* is_renderer_initiated */);
      open_params.user_gesture = params.mimic_user_gesture;

      if (delegate_ && !is_guest &&
          !delegate_->ShouldResumeRequestsForCreatedWindow()) {
        DCHECK(weak_new_contents);
        weak_new_contents->delayed_open_url_params_.reset(
            new OpenURLParams(open_params));
      } else {
        weak_new_contents->OpenURL(open_params);
      }
    }
  }
}
