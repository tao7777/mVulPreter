void ChromeRenderProcessObserver::OnWriteTcmallocHeapProfile(
    const FilePath::StringType& filename) {
#if !defined(OS_WIN)
  if (!IsHeapProfilerRunning())
    return;
  char* profile = GetHeapProfile();
  if (!profile) {
    LOG(WARNING) << "Unable to get heap profile.";
    return;
  }
  std::string result(profile);
  delete profile;
  RenderThread::Get()->Send(
      new ChromeViewHostMsg_WriteTcmallocHeapProfile_ACK(filename, result));
#endif
}
