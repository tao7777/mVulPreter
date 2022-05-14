bool ChromeRenderProcessObserver::OnControlMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ChromeRenderProcessObserver, message)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_SetIsIncognitoProcess,
                        OnSetIsIncognitoProcess)
     IPC_MESSAGE_HANDLER(ChromeViewMsg_SetCacheCapacities, OnSetCacheCapacities)
     IPC_MESSAGE_HANDLER(ChromeViewMsg_ClearCache, OnClearCache)
     IPC_MESSAGE_HANDLER(ChromeViewMsg_SetFieldTrialGroup, OnSetFieldTrialGroup)
#if defined(USE_TCMALLOC)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_SetTcmallocHeapProfiling,
                        OnSetTcmallocHeapProfiling)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_WriteTcmallocHeapProfile,
                        OnWriteTcmallocHeapProfile)
#endif
     IPC_MESSAGE_HANDLER(ChromeViewMsg_GetV8HeapStats, OnGetV8HeapStats)
     IPC_MESSAGE_HANDLER(ChromeViewMsg_GetCacheResourceStats,
                         OnGetCacheResourceStats)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_PurgeMemory, OnPurgeMemory)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_SetContentSettingRules,
                        OnSetContentSettingRules)
    IPC_MESSAGE_HANDLER(ChromeViewMsg_ToggleWebKitSharedTimer,
                        OnToggleWebKitSharedTimer)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}
