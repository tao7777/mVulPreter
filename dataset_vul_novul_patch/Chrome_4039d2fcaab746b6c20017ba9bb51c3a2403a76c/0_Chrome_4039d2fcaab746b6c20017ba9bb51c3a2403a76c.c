 bool RenderFrameImpl::OnMessageReceived(const IPC::Message& msg) {
  GetContentClient()->SetActiveURL(frame_->document().url());

   ObserverListBase<RenderFrameObserver>::Iterator it(observers_);
   RenderFrameObserver* observer;
   while ((observer = it.GetNext()) != NULL) {
    if (observer->OnMessageReceived(msg))
      return true;
  }

  bool handled = true;
  bool msg_is_ok = true;
  IPC_BEGIN_MESSAGE_MAP_EX(RenderFrameImpl, msg, msg_is_ok)
    IPC_MESSAGE_HANDLER(FrameMsg_Navigate, OnNavigate)
    IPC_MESSAGE_HANDLER(FrameMsg_BeforeUnload, OnBeforeUnload)
    IPC_MESSAGE_HANDLER(FrameMsg_SwapOut, OnSwapOut)
    IPC_MESSAGE_HANDLER(FrameMsg_BuffersSwapped, OnBuffersSwapped)
    IPC_MESSAGE_HANDLER_GENERIC(FrameMsg_CompositorFrameSwapped,
                                OnCompositorFrameSwapped(msg))
    IPC_MESSAGE_HANDLER(FrameMsg_ChildFrameProcessGone, OnChildFrameProcessGone)
    IPC_MESSAGE_HANDLER(FrameMsg_ContextMenuClosed, OnContextMenuClosed)
    IPC_MESSAGE_HANDLER(FrameMsg_CustomContextMenuAction,
                        OnCustomContextMenuAction)
    IPC_MESSAGE_HANDLER(InputMsg_Undo, OnUndo)
    IPC_MESSAGE_HANDLER(InputMsg_Redo, OnRedo)
    IPC_MESSAGE_HANDLER(InputMsg_Cut, OnCut)
    IPC_MESSAGE_HANDLER(InputMsg_Copy, OnCopy)
    IPC_MESSAGE_HANDLER(InputMsg_Paste, OnPaste)
    IPC_MESSAGE_HANDLER(InputMsg_PasteAndMatchStyle, OnPasteAndMatchStyle)
    IPC_MESSAGE_HANDLER(InputMsg_Delete, OnDelete)
    IPC_MESSAGE_HANDLER(InputMsg_SelectAll, OnSelectAll)
    IPC_MESSAGE_HANDLER(InputMsg_SelectRange, OnSelectRange)
    IPC_MESSAGE_HANDLER(InputMsg_Unselect, OnUnselect)
    IPC_MESSAGE_HANDLER(InputMsg_Replace, OnReplace)
    IPC_MESSAGE_HANDLER(InputMsg_ReplaceMisspelling, OnReplaceMisspelling)
    IPC_MESSAGE_HANDLER(FrameMsg_CSSInsertRequest, OnCSSInsertRequest)
    IPC_MESSAGE_HANDLER(FrameMsg_JavaScriptExecuteRequest,
                        OnJavaScriptExecuteRequest)
    IPC_MESSAGE_HANDLER(FrameMsg_SetEditableSelectionOffsets,
                        OnSetEditableSelectionOffsets)
    IPC_MESSAGE_HANDLER(FrameMsg_SetCompositionFromExistingText,
                        OnSetCompositionFromExistingText)
    IPC_MESSAGE_HANDLER(FrameMsg_ExtendSelectionAndDelete,
                        OnExtendSelectionAndDelete)
#if defined(OS_MACOSX)
    IPC_MESSAGE_HANDLER(InputMsg_CopyToFindPboard, OnCopyToFindPboard)
#endif
    IPC_MESSAGE_HANDLER(FrameMsg_Reload, OnReload)
  IPC_END_MESSAGE_MAP_EX()

   if (!msg_is_ok) {
    int id = msg.type();
    CHECK(false) << "Unable to deserialize " << id << " in RenderFrameImpl.";
   }
 
   return handled;
}
