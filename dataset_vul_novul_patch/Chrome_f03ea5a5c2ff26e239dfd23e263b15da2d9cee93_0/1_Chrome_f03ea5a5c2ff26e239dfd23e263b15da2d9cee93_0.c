bool RenderFrameHostImpl::OnMessageReceived(const IPC::Message &msg) {
  if (!render_frame_created_)
    return false;

  ScopedActiveURL scoped_active_url(this);

  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderFrameHostImpl, msg)
    IPC_MESSAGE_HANDLER(FrameHostMsg_RenderProcessGone, OnRenderProcessGone)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  if (handled)
    return true;

  if (delegate_->OnMessageReceived(this, msg))
    return true;

  RenderFrameProxyHost* proxy =
      frame_tree_node_->render_manager()->GetProxyToParent();
  if (proxy && proxy->cross_process_frame_connector() &&
      proxy->cross_process_frame_connector()->OnMessageReceived(msg))
    return true;
 
   handled = true;
   IPC_BEGIN_MESSAGE_MAP(RenderFrameHostImpl, msg)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidAddMessageToConsole,
                        OnDidAddMessageToConsole)
     IPC_MESSAGE_HANDLER(FrameHostMsg_Detach, OnDetach)
     IPC_MESSAGE_HANDLER(FrameHostMsg_FrameFocused, OnFrameFocused)
     IPC_MESSAGE_HANDLER(FrameHostMsg_DidFailProvisionalLoadWithError,
                        OnDidFailProvisionalLoadWithError)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidFailLoadWithError,
                        OnDidFailLoadWithError)
    IPC_MESSAGE_HANDLER(FrameHostMsg_UpdateState, OnUpdateState)
    IPC_MESSAGE_HANDLER(FrameHostMsg_OpenURL, OnOpenURL)
    IPC_MESSAGE_HANDLER(FrameHostMsg_BeforeUnload_ACK, OnBeforeUnloadACK)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SwapOut_ACK, OnSwapOutACK)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ContextMenu, OnContextMenu)
    IPC_MESSAGE_HANDLER(FrameHostMsg_VisualStateResponse,
                        OnVisualStateResponse)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(FrameHostMsg_RunJavaScriptDialog,
                                    OnRunJavaScriptDialog)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(FrameHostMsg_RunBeforeUnloadConfirm,
                                    OnRunBeforeUnloadConfirm)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidAccessInitialDocument,
                        OnDidAccessInitialDocument)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidChangeOpener, OnDidChangeOpener)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidAddContentSecurityPolicies,
                        OnDidAddContentSecurityPolicies)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidChangeFramePolicy,
                        OnDidChangeFramePolicy)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidChangeFrameOwnerProperties,
                        OnDidChangeFrameOwnerProperties)
    IPC_MESSAGE_HANDLER(FrameHostMsg_UpdateTitle, OnUpdateTitle)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidBlockFramebust, OnDidBlockFramebust)
    IPC_MESSAGE_HANDLER(FrameHostMsg_AbortNavigation, OnAbortNavigation)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DispatchLoad, OnDispatchLoad)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ForwardResourceTimingToParent,
                        OnForwardResourceTimingToParent)
    IPC_MESSAGE_HANDLER(FrameHostMsg_TextSurroundingSelectionResponse,
                        OnTextSurroundingSelectionResponse)
    IPC_MESSAGE_HANDLER(AccessibilityHostMsg_EventBundle, OnAccessibilityEvents)
    IPC_MESSAGE_HANDLER(AccessibilityHostMsg_LocationChanges,
                        OnAccessibilityLocationChanges)
    IPC_MESSAGE_HANDLER(AccessibilityHostMsg_FindInPageResult,
                        OnAccessibilityFindInPageResult)
    IPC_MESSAGE_HANDLER(AccessibilityHostMsg_ChildFrameHitTestResult,
                        OnAccessibilityChildFrameHitTestResult)
    IPC_MESSAGE_HANDLER(AccessibilityHostMsg_SnapshotResponse,
                        OnAccessibilitySnapshotResponse)
    IPC_MESSAGE_HANDLER(FrameHostMsg_EnterFullscreen, OnEnterFullscreen)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ExitFullscreen, OnExitFullscreen)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SuddenTerminationDisablerChanged,
                        OnSuddenTerminationDisablerChanged)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidStopLoading, OnDidStopLoading)
    IPC_MESSAGE_HANDLER(FrameHostMsg_DidChangeLoadProgress,
                        OnDidChangeLoadProgress)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SelectionChanged, OnSelectionChanged)
    IPC_MESSAGE_HANDLER(FrameHostMsg_FocusedNodeChanged, OnFocusedNodeChanged)
    IPC_MESSAGE_HANDLER(FrameHostMsg_UpdateUserActivationState,
                        OnUpdateUserActivationState)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SetHasReceivedUserGestureBeforeNavigation,
                        OnSetHasReceivedUserGestureBeforeNavigation)
    IPC_MESSAGE_HANDLER(FrameHostMsg_SetNeedsOcclusionTracking,
                        OnSetNeedsOcclusionTracking);
    IPC_MESSAGE_HANDLER(FrameHostMsg_ScrollRectToVisibleInParentFrame,
                        OnScrollRectToVisibleInParentFrame)
    IPC_MESSAGE_HANDLER(FrameHostMsg_BubbleLogicalScrollInParentFrame,
                        OnBubbleLogicalScrollInParentFrame)
    IPC_MESSAGE_HANDLER(FrameHostMsg_FrameDidCallFocus, OnFrameDidCallFocus)
    IPC_MESSAGE_HANDLER(FrameHostMsg_RenderFallbackContentInParentProcess,
                        OnRenderFallbackContentInParentProcess)
#if BUILDFLAG(USE_EXTERNAL_POPUP_MENU)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ShowPopup, OnShowPopup)
    IPC_MESSAGE_HANDLER(FrameHostMsg_HidePopup, OnHidePopup)
#endif
    IPC_MESSAGE_HANDLER(FrameHostMsg_RequestOverlayRoutingToken,
                        OnRequestOverlayRoutingToken)
    IPC_MESSAGE_HANDLER(FrameHostMsg_ShowCreatedWindow, OnShowCreatedWindow)
  IPC_END_MESSAGE_MAP()

  return handled;
}
