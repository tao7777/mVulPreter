 static void CopyTransportDIBHandleForMessage(
     const TransportDIB::Handle& handle_in,
    TransportDIB::Handle* handle_out,
    base::ProcessId peer_pid) {
 #if defined(OS_MACOSX)
  if ((handle_out->fd = HANDLE_EINTR(dup(handle_in.fd))) < 0) {
    PLOG(ERROR) << "dup()";
     return;
   }
   handle_out->auto_close = true;
#elif defined(OS_WIN)
  // On Windows we need to duplicate the handle for the plugin process.
  *handle_out = NULL;
  sandbox::BrokerDuplicateHandle(handle_in, peer_pid, handle_out,
                                 FILE_MAP_READ | FILE_MAP_WRITE, 0);
  CHECK(*handle_out != NULL);
 #else
   *handle_out = handle_in;
#endif
}
