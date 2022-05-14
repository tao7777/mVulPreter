ssize_t NaClDescCustomRecvMsg(void* handle, NaClImcTypedMsgHdr* msg,
                               int /* flags */) {
   if (msg->iov_length != 1)
     return -1;
  msg->ndesc_length = 0;  // Messages with descriptors aren't supported yet.
   return static_cast<ssize_t>(
       ToAdapter(handle)->BlockingReceive(static_cast<char*>(msg->iov[0].base),
                                          msg->iov[0].length));
}
