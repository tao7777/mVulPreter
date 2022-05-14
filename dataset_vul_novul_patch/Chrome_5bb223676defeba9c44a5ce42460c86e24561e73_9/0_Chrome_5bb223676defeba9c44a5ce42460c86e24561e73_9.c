void ExtensionsGuestViewMessageFilter::MimeHandlerViewGuestCreatedCallback(
    int element_instance_id,
    int embedder_render_process_id,
    int embedder_render_frame_id,
    int32_t plugin_frame_routing_id,
    const gfx::Size& element_size,
    mime_handler::BeforeUnloadControlPtrInfo before_unload_control,
    bool is_full_page_plugin,
    WebContents* web_contents) {
  auto* guest_view = MimeHandlerViewGuest::FromWebContents(web_contents);
  if (!guest_view)
    return;

  guest_view->SetBeforeUnloadController(std::move(before_unload_control));
  int guest_instance_id = guest_view->guest_instance_id();
  auto* rfh = RenderFrameHost::FromID(embedder_render_process_id,
                                      embedder_render_frame_id);
  if (!rfh)
    return;

  guest_view->SetEmbedderFrame(embedder_render_process_id,
                               embedder_render_frame_id);

  base::DictionaryValue attach_params;
  attach_params.SetInteger(guest_view::kElementWidth, element_size.width());
  attach_params.SetInteger(guest_view::kElementHeight, element_size.height());
  auto* manager = GuestViewManager::FromBrowserContext(browser_context_);
  if (!manager) {
    guest_view::bad_message::ReceivedBadMessage(
        this,
        guest_view::bad_message::GVMF_UNEXPECTED_MESSAGE_BEFORE_GVM_CREATION);
    guest_view->Destroy(true);
    return;
  }
  manager->AttachGuest(embedder_render_process_id, element_instance_id,
                       guest_instance_id, attach_params);

  if (!content::MimeHandlerViewMode::UsesCrossProcessFrame()) {
    rfh->Send(new ExtensionsGuestViewMsg_CreateMimeHandlerViewGuestACK(
         element_instance_id));
     return;
   }
