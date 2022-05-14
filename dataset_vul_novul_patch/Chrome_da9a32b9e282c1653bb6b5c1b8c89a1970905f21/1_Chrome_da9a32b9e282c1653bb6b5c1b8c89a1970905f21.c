 bool RenderFrameHostImpl::OnMessageReceived(const IPC::Message &msg) {
   if (delegate_->OnMessageReceived(this, msg))
     return true;
 
  if (cross_process_frame_connector_ &&
      cross_process_frame_connector_->OnMessageReceived(msg))
    return true;

  bool handled = true;
  bool msg_is_ok = true;
  IPC_BEGIN_MESSAGE_MAP_EX(RenderFrameHostImpl, msg, msg_is_ok)
    IPC_MESSAGE_HANDLER(FrameHostMsg_Detach, OnDetach)
    IPC_MESSAGE_HANDLER(FrameHostMsg_FrameFocused, OnFrameFocused)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidStartProvisionalLoadForFrame,
                        OnDidStartProvisionalLoadForFrame)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidFailProvisionalLoadWithError,
                        OnDidFailProvisionalLoadWithError)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidRedirectProvisionalLoad,
                        OnDidRedirectProvisionalLoad)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidFailLoadWithError,
                        OnDidFailLoadWithError)
    IPC_MESSAGE_HANDLER_GENERIC(FrameHostMsg_DidCommitProvisionalLoad,
                                OnNavigate(msg))
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidStartLoading, OnDidStartLoading)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidStopLoading, OnDidStopLoading)
    IPC_MESSAGE_HANDLER(FrameHostMsg_OpenURL, OnOpenURL)
    IPC_MESSAGE_HANDLER(FrameHostMsg_BeforeUnload_ACK, OnBeforeUnloadACK)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SwapOut_ACK, OnSwapOutACK)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ContextMenu, OnContextMenu)
    IPC_MESSAGE_HANDLER(FrameHostMsg_JavaScriptExecuteResponse,
                        OnJavaScriptExecuteResponse)
  IPC_END_MESSAGE_MAP_EX()

  if (!msg_is_ok) {
    RecordAction(base::UserMetricsAction("BadMessageTerminate_RFH"));
    GetProcess()->ReceivedBadMessage();
  }

  return handled;
}
