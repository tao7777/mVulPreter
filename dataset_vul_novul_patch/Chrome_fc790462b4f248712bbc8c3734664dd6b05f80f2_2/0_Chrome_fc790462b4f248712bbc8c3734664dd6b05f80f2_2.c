bool ResourceMessageFilter::OnMessageReceived(const IPC::Message& msg) {
  MessagePortDispatcher* mp_dispatcher = MessagePortDispatcher::GetInstance();
  bool msg_is_ok = true;
  bool handled =
      resource_dispatcher_host_->OnMessageReceived(msg, this, &msg_is_ok) ||
      appcache_dispatcher_host_->OnMessageReceived(msg, &msg_is_ok) ||
      dom_storage_dispatcher_host_->OnMessageReceived(msg, &msg_is_ok) ||
      audio_renderer_host_->OnMessageReceived(msg, &msg_is_ok) ||
      db_dispatcher_host_->OnMessageReceived(msg, &msg_is_ok) ||
      mp_dispatcher->OnMessageReceived(
          msg, this, next_route_id_callback(), &msg_is_ok) ||
      geolocation_dispatcher_host_->OnMessageReceived(msg, &msg_is_ok);

  if (!handled) {
    DCHECK(msg_is_ok);  // It should have been marked handled if it wasn't OK.
    handled = true;
    IPC_BEGIN_MESSAGE_MAP_EX(ResourceMessageFilter, msg, msg_is_ok)
#if !defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetScreenInfo,
                                      OnGetScreenInfo)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetWindowRect,
                                      OnGetWindowRect)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetRootWindowRect,
                                      OnGetRootWindowRect)
#endif

      IPC_MESSAGE_HANDLER(ViewHostMsg_CreateWindow, OnMsgCreateWindow)
       IPC_MESSAGE_HANDLER(ViewHostMsg_CreateWidget, OnMsgCreateWidget)
       IPC_MESSAGE_HANDLER(ViewHostMsg_SetCookie, OnSetCookie)
       IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetCookies, OnGetCookies)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetRawCookies,
                                      OnGetRawCookies)
       IPC_MESSAGE_HANDLER(ViewHostMsg_DeleteCookie, OnDeleteCookie)
       IPC_MESSAGE_HANDLER(ViewHostMsg_GetCookiesEnabled, OnGetCookiesEnabled)
 #if defined(OS_WIN)  // This hack is Windows-specific.
      IPC_MESSAGE_HANDLER(ViewHostMsg_PreCacheFont, OnPreCacheFont)
#endif
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetPlugins, OnGetPlugins)
      IPC_MESSAGE_HANDLER(ViewHostMsg_GetPluginPath, OnGetPluginPath)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DownloadUrl, OnDownloadUrl)
      IPC_MESSAGE_HANDLER_GENERIC(ViewHostMsg_ContextMenu,
                                  OnReceiveContextMenuMsg(msg))
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_OpenChannelToPlugin,
                                      OnOpenChannelToPlugin)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_LaunchNaCl, OnLaunchNaCl)
      IPC_MESSAGE_HANDLER(ViewHostMsg_CreateWorker, OnCreateWorker)
      IPC_MESSAGE_HANDLER(ViewHostMsg_LookupSharedWorker, OnLookupSharedWorker)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DocumentDetached, OnDocumentDetached)
      IPC_MESSAGE_HANDLER(ViewHostMsg_CancelCreateDedicatedWorker,
                          OnCancelCreateDedicatedWorker)
      IPC_MESSAGE_HANDLER(ViewHostMsg_ForwardToWorker,
                          OnForwardToWorker)
      IPC_MESSAGE_HANDLER(ViewHostMsg_SpellChecker_PlatformCheckSpelling,
                          OnPlatformCheckSpelling)
      IPC_MESSAGE_HANDLER(ViewHostMsg_SpellChecker_PlatformFillSuggestionList,
                          OnPlatformFillSuggestionList)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetDocumentTag,
                                      OnGetDocumentTag)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DocumentWithTagClosed,
                          OnDocumentWithTagClosed)
      IPC_MESSAGE_HANDLER(ViewHostMsg_ShowSpellingPanel, OnShowSpellingPanel)
      IPC_MESSAGE_HANDLER(ViewHostMsg_UpdateSpellingPanelWithMisspelledWord,
                          OnUpdateSpellingPanelWithMisspelledWord)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DnsPrefetch, OnDnsPrefetch)
      IPC_MESSAGE_HANDLER(ViewHostMsg_RendererHistograms,
                          OnRendererHistograms)
      IPC_MESSAGE_HANDLER_GENERIC(ViewHostMsg_UpdateRect,
          render_widget_helper_->DidReceiveUpdateMsg(msg))
      IPC_MESSAGE_HANDLER(ViewHostMsg_ClipboardWriteObjectsAsync,
                          OnClipboardWriteObjectsAsync)
      IPC_MESSAGE_HANDLER(ViewHostMsg_ClipboardWriteObjectsSync,
                          OnClipboardWriteObjectsSync)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ClipboardIsFormatAvailable,
                                      OnClipboardIsFormatAvailable)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ClipboardReadText,
                                      OnClipboardReadText)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ClipboardReadAsciiText,
                                      OnClipboardReadAsciiText)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ClipboardReadHTML,
                                      OnClipboardReadHTML)
#if defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER(ViewHostMsg_ClipboardFindPboardWriteStringAsync,
                          OnClipboardFindPboardWriteString)
#endif
      IPC_MESSAGE_HANDLER(ViewHostMsg_CheckNotificationPermission,
                          OnCheckNotificationPermission)
      IPC_MESSAGE_HANDLER(ViewHostMsg_GetMimeTypeFromExtension,
                          OnGetMimeTypeFromExtension)
      IPC_MESSAGE_HANDLER(ViewHostMsg_GetMimeTypeFromFile,
                          OnGetMimeTypeFromFile)
      IPC_MESSAGE_HANDLER(ViewHostMsg_GetPreferredExtensionForMimeType,
                          OnGetPreferredExtensionForMimeType)
      IPC_MESSAGE_HANDLER(ViewHostMsg_GetCPBrowsingContext,
                          OnGetCPBrowsingContext)
#if defined(OS_WIN)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DuplicateSection, OnDuplicateSection)
#endif
#if defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER(ViewHostMsg_AllocatePDFTransport,
                          OnAllocateSharedMemoryBuffer)
#endif
#if defined(OS_POSIX)
      IPC_MESSAGE_HANDLER(ViewHostMsg_AllocateSharedMemoryBuffer,
                          OnAllocateSharedMemoryBuffer)
#endif
#if defined(OS_POSIX) && !defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_AllocateTempFileForPrinting,
                                      OnAllocateTempFileForPrinting)
      IPC_MESSAGE_HANDLER(ViewHostMsg_TempFileForPrintingWritten,
                          OnTempFileForPrintingWritten)
#endif
      IPC_MESSAGE_HANDLER(ViewHostMsg_ResourceTypeStats, OnResourceTypeStats)
      IPC_MESSAGE_HANDLER(ViewHostMsg_V8HeapStats, OnV8HeapStats)
      IPC_MESSAGE_HANDLER(ViewHostMsg_DidZoomURL, OnDidZoomURL)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ResolveProxy, OnResolveProxy)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetDefaultPrintSettings,
                                      OnGetDefaultPrintSettings)
#if defined(OS_WIN) || defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_ScriptedPrint,
                                      OnScriptedPrint)
#endif
#if defined(OS_MACOSX)
      IPC_MESSAGE_HANDLER(ViewHostMsg_AllocTransportDIB,
                          OnAllocTransportDIB)
      IPC_MESSAGE_HANDLER(ViewHostMsg_FreeTransportDIB,
                          OnFreeTransportDIB)
#endif
      IPC_MESSAGE_HANDLER(ViewHostMsg_OpenChannelToExtension,
                          OnOpenChannelToExtension)
      IPC_MESSAGE_HANDLER(ViewHostMsg_OpenChannelToTab, OnOpenChannelToTab)
      IPC_MESSAGE_HANDLER(ViewHostMsg_CloseCurrentConnections,
                          OnCloseCurrentConnections)
      IPC_MESSAGE_HANDLER(ViewHostMsg_SetCacheMode, OnSetCacheMode)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetFileSize, OnGetFileSize)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetFileModificationTime,
                                      OnGetFileModificationTime)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_OpenFile, OnOpenFile)
      IPC_MESSAGE_HANDLER(ViewHostMsg_Keygen, OnKeygen)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_GetExtensionMessageBundle,
                                      OnGetExtensionMessageBundle)
#if defined(USE_TCMALLOC)
      IPC_MESSAGE_HANDLER(ViewHostMsg_RendererTcmalloc, OnRendererTcmalloc)
#endif
      IPC_MESSAGE_HANDLER(ViewHostMsg_EstablishGpuChannel,
                          OnEstablishGpuChannel)
      IPC_MESSAGE_HANDLER_DELAY_REPLY(ViewHostMsg_SynchronizeGpu,
                                      OnSynchronizeGpu)
      IPC_MESSAGE_UNHANDLED(
          handled = false)
    IPC_END_MESSAGE_MAP_EX()
  }

  if (!msg_is_ok)
    BrowserRenderProcessHost::BadMessageTerminateProcess(msg.type(), handle());

  return handled;
}
