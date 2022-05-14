 static void CopyTransportDIBHandleForMessage(
     const TransportDIB::Handle& handle_in,
    TransportDIB::Handle* handle_out) {
 #if defined(OS_MACOSX)
  if ((handle_out->fd = HANDLE_EINTR(dup(handle_in.fd))) < 0) {
    PLOG(ERROR) << "dup()";
     return;
   }
   handle_out->auto_close = true;
 #else
   *handle_out = handle_in;
#endif
}
