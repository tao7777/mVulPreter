void NaClProcessHost::OnProcessLaunched() {
  FilePath irt_path;
  const char* irt_path_var = getenv("NACL_IRT_LIBRARY");
  if (irt_path_var != NULL) {
    FilePath::StringType string(irt_path_var,
                                irt_path_var + strlen(irt_path_var));
    irt_path = FilePath(string);
  } else {
    FilePath plugin_dir;
    if (!PathService::Get(chrome::DIR_INTERNAL_PLUGINS, &plugin_dir)) {
      LOG(ERROR) << "Failed to locate the plugins directory";
      delete this;
      return;
    }
    irt_path = plugin_dir.Append(GetIrtLibraryFilename());
  }

  base::FileUtilProxy::CreateOrOpenCallback* callback =
      callback_factory_.NewCallback(&NaClProcessHost::OpenIrtFileDone);
  if (!base::FileUtilProxy::CreateOrOpen(
           BrowserThread::GetMessageLoopProxyForThread(BrowserThread::FILE),
            irt_path,
            base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ,
            callback)) {
    delete callback;
     delete this;
   }
 }
