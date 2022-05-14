void WebContentsImpl::RunJavaScriptDialog(RenderFrameHost* render_frame_host,
                                          const base::string16& message,
                                          const base::string16& default_prompt,
                                           const GURL& frame_url,
                                           JavaScriptDialogType dialog_type,
                                           IPC::Message* reply_msg) {
  bool suppress_this_message =
      ShowingInterstitialPage() || !delegate_ ||
      delegate_->ShouldSuppressDialogs(this) ||
      !delegate_->GetJavaScriptDialogManager(this);

  if (!suppress_this_message) {
    is_showing_javascript_dialog_ = true;
    dialog_manager_ = delegate_->GetJavaScriptDialogManager(this);
    dialog_manager_->RunJavaScriptDialog(
        this, frame_url, dialog_type, message, default_prompt,
        base::Bind(&WebContentsImpl::OnDialogClosed, base::Unretained(this),
                   render_frame_host->GetProcess()->GetID(),
                   render_frame_host->GetRoutingID(), reply_msg, false),
        &suppress_this_message);
  }

  if (suppress_this_message) {
    OnDialogClosed(render_frame_host->GetProcess()->GetID(),
                   render_frame_host->GetRoutingID(), reply_msg,
                   true, false, base::string16());
  }
}
