void ResourceMessageFilter::DoOnAllocateTempFileForPrinting(
    IPC::Message* reply_msg) {
  base::FileDescriptor temp_file_fd;
  int fd_in_browser;
   temp_file_fd.fd = fd_in_browser = -1;
   temp_file_fd.auto_close = false;
 
  bool allow_print =
#if defined(TOOLKIT_GTK)
    !PrintDialogGtk::DialogShowing();
#else
    true;
#endif
   FilePath path;
  if (allow_print &&
      file_util::CreateTemporaryFile(&path)) {
     int fd = open(path.value().c_str(), O_WRONLY);
     if (fd >= 0) {
       FdMap* map = &Singleton<PrintingFileDescriptorMap>::get()->map;
      FdMap::iterator it = map->find(fd);
      if (it != map->end()) {
        NOTREACHED() << "The file descriptor is in use. fd=" << fd;
      } else {
        (*map)[fd] = path;
        temp_file_fd.fd = fd_in_browser = fd;
        temp_file_fd.auto_close = true;
      }
    }
  }

  ViewHostMsg_AllocateTempFileForPrinting::WriteReplyParams(
      reply_msg, temp_file_fd, fd_in_browser);

   ChromeThread::PostTask(
      ChromeThread::IO, FROM_HERE,
      NewRunnableMethod(
          this, &ResourceMessageFilter::SendDelayedReply, reply_msg));
}
