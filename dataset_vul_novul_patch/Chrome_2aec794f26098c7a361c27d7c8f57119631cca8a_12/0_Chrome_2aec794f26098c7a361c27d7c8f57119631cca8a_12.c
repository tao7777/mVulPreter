void RenderFrameDevToolsAgentHost::DestroyOnRenderFrameGone() {
   scoped_refptr<RenderFrameDevToolsAgentHost> protect(this);
   if (IsAttached())
     RevokePolicy();
  ForceDetachAllSessions();
   frame_host_ = nullptr;
   agent_ptr_.reset();
   SetFrameTreeNode(nullptr);
  Release();
}
