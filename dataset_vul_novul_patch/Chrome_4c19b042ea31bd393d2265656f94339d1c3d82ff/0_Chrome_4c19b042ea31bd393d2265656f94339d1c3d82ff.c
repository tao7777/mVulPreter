bool FileUtilProxy::Read(
    scoped_refptr<MessageLoopProxy> message_loop_proxy,
    PlatformFile file,
     int64 offset,
     int bytes_to_read,
     ReadCallback* callback) {
  if (bytes_to_read < 0) {
    delete callback;
     return false;
  }
   return Start(FROM_HERE, message_loop_proxy,
                new RelayRead(file, offset, bytes_to_read, callback));
 }
