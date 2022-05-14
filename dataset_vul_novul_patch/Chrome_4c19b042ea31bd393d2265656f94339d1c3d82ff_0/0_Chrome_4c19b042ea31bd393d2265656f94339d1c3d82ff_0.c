bool FileUtilProxy::Write(
    scoped_refptr<MessageLoopProxy> message_loop_proxy,
    PlatformFile file,
    int64 offset,
     const char* buffer,
     int bytes_to_write,
     WriteCallback* callback) {
  if (bytes_to_write <= 0) {
    delete callback;
     return false;
  }
   return Start(FROM_HERE, message_loop_proxy,
                new RelayWrite(file, offset, buffer, bytes_to_write, callback));
 }
