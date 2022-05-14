void RenderFrameDevToolsAgentHost::UpdateFrameHost(
    RenderFrameHostImpl* frame_host) {
  if (frame_host == frame_host_) {
    if (frame_host && !render_frame_alive_) {
      render_frame_alive_ = true;
      for (auto* inspector : protocol::InspectorHandler::ForAgentHost(this))
        inspector->TargetReloadedAfterCrash();
      MaybeReattachToRenderFrame();
    }
    return;
  }

  if (frame_host && !ShouldCreateDevToolsForHost(frame_host)) {
    DestroyOnRenderFrameGone();
    return;
  }
 
   if (IsAttached())
     RevokePolicy();
   frame_host_ = frame_host;
   agent_ptr_.reset();
   if (!render_frame_alive_) {
     render_frame_alive_ = true;
     for (auto* inspector : protocol::InspectorHandler::ForAgentHost(this))
       inspector->TargetReloadedAfterCrash();
   }
   if (IsAttached()) {
     GrantPolicy();
     for (DevToolsSession* session : sessions()) {
      session->SetRenderer(frame_host ? frame_host->GetProcess()->GetID() : -1,
                           frame_host);
    }
    MaybeReattachToRenderFrame();
  }
}
