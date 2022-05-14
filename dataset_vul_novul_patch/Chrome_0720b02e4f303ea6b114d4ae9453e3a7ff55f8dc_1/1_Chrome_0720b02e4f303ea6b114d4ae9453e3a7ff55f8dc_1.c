void WebContentsImpl::RunBeforeUnloadConfirm(
     RenderFrameHost* render_frame_host,
     bool is_reload,
     IPC::Message* reply_msg) {
   RenderFrameHostImpl* rfhi =
       static_cast<RenderFrameHostImpl*>(render_frame_host);
   if (delegate_)
    delegate_->WillRunBeforeUnloadConfirm();

  bool suppress_this_message =
      !rfhi->is_active() ||
      ShowingInterstitialPage() || !delegate_ ||
      delegate_->ShouldSuppressDialogs(this) ||
      !delegate_->GetJavaScriptDialogManager(this);
  if (suppress_this_message) {
    rfhi->JavaScriptDialogClosed(reply_msg, true, base::string16());
    return;
  }

  is_showing_before_unload_dialog_ = true;
  dialog_manager_ = delegate_->GetJavaScriptDialogManager(this);
  dialog_manager_->RunBeforeUnloadDialog(
      this, is_reload,
      base::Bind(&WebContentsImpl::OnDialogClosed, base::Unretained(this),
                 render_frame_host->GetProcess()->GetID(),
                 render_frame_host->GetRoutingID(), reply_msg,
                 false));
}
