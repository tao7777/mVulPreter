bool ChromeContentUtilityClient::OnMessageReceived(
    const IPC::Message& message) {
  if (filter_messages_ && !ContainsKey(message_id_whitelist_, message.type()))
    return false;

  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ChromeContentUtilityClient, message)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_DecodeImage, OnDecodeImage)
#if defined(OS_CHROMEOS)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_RobustJPEGDecodeImage,
                        OnRobustJPEGDecodeImage)
#endif  // defined(OS_CHROMEOS)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_PatchFileBsdiff,
                        OnPatchFileBsdiff)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_PatchFileCourgette,
                        OnPatchFileCourgette)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_StartupPing, OnStartupPing)
 #if defined(FULL_SAFE_BROWSING)
     IPC_MESSAGE_HANDLER(ChromeUtilityMsg_AnalyzeZipFileForDownloadProtection,
                         OnAnalyzeZipFileForDownloadProtection)
 #endif
 #if defined(ENABLE_EXTENSIONS)
     IPC_MESSAGE_HANDLER(ChromeUtilityMsg_ParseMediaMetadata,
                        OnParseMediaMetadata)
#endif
#if defined(OS_CHROMEOS)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_CreateZipFile, OnCreateZipFile)
#endif
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  for (Handlers::iterator it = handlers_.begin();
       !handled && it != handlers_.end(); ++it) {
    handled = (*it)->OnMessageReceived(message);
  }

  return handled;
}
