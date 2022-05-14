bool ChromeRenderMessageFilter::OnMessageReceived(const IPC::Message& message,
                                                  bool* message_was_ok) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP_EX(ChromeRenderMessageFilter, message, *message_was_ok)
#if !defined(DISABLE_NACL)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(ChromeViewHostMsg_LaunchNaCl, OnLaunchNaCl)
#endif
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_DnsPrefetch, OnDnsPrefetch)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_RendererHistograms,
                        OnRendererHistograms)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_ResourceTypeStats,
                        OnResourceTypeStats)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_UpdatedCacheStats,
                        OnUpdatedCacheStats)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_FPS, OnFPS)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_V8HeapStats, OnV8HeapStats)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_OpenChannelToExtension,
                        OnOpenChannelToExtension)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_OpenChannelToTab, OnOpenChannelToTab)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(ExtensionHostMsg_GetMessageBundle,
                                    OnGetExtensionMessageBundle)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_AddListener, OnExtensionAddListener)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_RemoveListener,
                        OnExtensionRemoveListener)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_AddLazyListener,
                        OnExtensionAddLazyListener)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_RemoveLazyListener,
                        OnExtensionRemoveLazyListener)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_CloseChannel, OnExtensionCloseChannel)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_RequestForIOThread,
                        OnExtensionRequestForIOThread)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_ShouldUnloadAck,
                        OnExtensionShouldUnloadAck)
     IPC_MESSAGE_HANDLER(ExtensionHostMsg_GenerateUniqueID,
                         OnExtensionGenerateUniqueID)
     IPC_MESSAGE_HANDLER(ExtensionHostMsg_UnloadAck, OnExtensionUnloadAck)
    IPC_MESSAGE_HANDLER(ExtensionHostMsg_ResumeRequests,
                        OnExtensionResumeRequests);
 #if defined(USE_TCMALLOC)
     IPC_MESSAGE_HANDLER(ChromeViewHostMsg_WriteTcmallocHeapProfile_ACK,
                         OnWriteTcmallocHeapProfile)
#endif
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowDatabase, OnAllowDatabase)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowDOMStorage, OnAllowDOMStorage)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowFileSystem, OnAllowFileSystem)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowIndexedDB, OnAllowIndexedDB)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_CanTriggerClipboardRead,
                        OnCanTriggerClipboardRead)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_CanTriggerClipboardWrite,
                        OnCanTriggerClipboardWrite)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

#if defined(ENABLE_AUTOMATION)
  if ((message.type() == ChromeViewHostMsg_GetCookies::ID ||
       message.type() == ChromeViewHostMsg_SetCookie::ID) &&
    AutomationResourceMessageFilter::ShouldFilterCookieMessages(
        render_process_id_, message.routing_id())) {
    IPC_BEGIN_MESSAGE_MAP_EX(ChromeRenderMessageFilter, message,
                             *message_was_ok)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ChromeViewHostMsg_GetCookies,
                                      OnGetCookies)
      IPC_MESSAGE_HANDLER(ChromeViewHostMsg_SetCookie, OnSetCookie)
    IPC_END_MESSAGE_MAP()
    handled = true;
  }
#endif

  return handled;
}
