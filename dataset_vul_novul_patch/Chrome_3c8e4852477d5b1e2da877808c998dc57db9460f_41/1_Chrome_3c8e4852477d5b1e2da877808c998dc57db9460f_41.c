void RenderFrameDevToolsAgentHost::UpdateFrameHost(
    RenderFrameHostImpl* frame_host) {
  if (frame_host == frame_host_) {
    if (frame_host && !render_frame_alive_) {
      render_frame_alive_ = true;
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
  render_frame_alive_ = true;
   if (IsAttached()) {
     GrantPolicy();
     for (DevToolsSession* session : sessions()) {
      session->SetRenderer(frame_host ? frame_host->GetProcess() : nullptr,
                            frame_host);
     }
     MaybeReattachToRenderFrame();
  }
}
