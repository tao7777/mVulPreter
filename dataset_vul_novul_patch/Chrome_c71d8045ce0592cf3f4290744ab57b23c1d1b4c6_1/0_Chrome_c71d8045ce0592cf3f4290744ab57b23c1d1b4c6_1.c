bool RenderFrameDevToolsAgentHost::AttachSession(DevToolsSession* session,
                                                 TargetRegistry* registry) {
  if (!ShouldAllowSession(session))
    return false;

  protocol::EmulationHandler* emulation_handler =
      new protocol::EmulationHandler();
  session->AddHandler(base::WrapUnique(new protocol::BrowserHandler()));
  session->AddHandler(base::WrapUnique(new protocol::DOMHandler()));
  session->AddHandler(base::WrapUnique(emulation_handler));
  session->AddHandler(base::WrapUnique(new protocol::InputHandler()));
  session->AddHandler(base::WrapUnique(new protocol::InspectorHandler()));
  session->AddHandler(base::WrapUnique(new protocol::IOHandler(
      GetIOContext())));
  session->AddHandler(base::WrapUnique(new protocol::MemoryHandler()));
  session->AddHandler(base::WrapUnique(new protocol::NetworkHandler(
      GetId(),
      frame_tree_node_ ? frame_tree_node_->devtools_frame_token()
                       : base::UnguessableToken(),
      GetIOContext())));
  session->AddHandler(base::WrapUnique(new protocol::SchemaHandler()));
  session->AddHandler(base::WrapUnique(new protocol::ServiceWorkerHandler()));
  session->AddHandler(base::WrapUnique(new protocol::StorageHandler()));
  session->AddHandler(base::WrapUnique(new protocol::TargetHandler(
      session->client()->MayDiscoverTargets()
           ? protocol::TargetHandler::AccessMode::kRegular
           : protocol::TargetHandler::AccessMode::kAutoAttachOnly,
       GetId(), registry)));
  session->AddHandler(base::WrapUnique(new protocol::PageHandler(
      emulation_handler, session->client()->MayAffectLocalFiles())));
   session->AddHandler(base::WrapUnique(new protocol::SecurityHandler()));
   if (!frame_tree_node_ || !frame_tree_node_->parent()) {
     session->AddHandler(base::WrapUnique(
        new protocol::TracingHandler(frame_tree_node_, GetIOContext())));
  }

  if (sessions().empty()) {
    bool use_video_capture_api = true;
#ifdef OS_ANDROID
    if (!CompositorImpl::IsInitialized())
      use_video_capture_api = false;
#endif
    if (!use_video_capture_api)
      frame_trace_recorder_.reset(new DevToolsFrameTraceRecorder());
    GrantPolicy();
#if defined(OS_ANDROID)
    GetWakeLock()->RequestWakeLock();
#endif
  }
  return true;
}
