void SandboxIPCHandler::HandleLocaltime(
     int fd,
     base::PickleIterator iter,
     const std::vector<base::ScopedFD>& fds) {
  // The other side of this call is in |ProxyLocaltimeCallToBrowser|, in
  // zygote_main_linux.cc.
 
   std::string time_string;
   if (!iter.ReadString(&time_string) || time_string.size() != sizeof(time_t))
     return;
 
   time_t time;
   memcpy(&time, time_string.data(), sizeof(time));
  // We use |localtime| here because we need the |tm_zone| field to be filled
   const struct tm* expanded_time = localtime(&time);
 
  base::Pickle reply;
   if (expanded_time) {
    WriteTimeStruct(&reply, expanded_time);
  } else {
    // The {} constructor ensures the struct is 0-initialized.
    struct tm zeroed_time = {};
    WriteTimeStruct(&reply, &zeroed_time);
   }
   SendRendererReply(fds, reply, -1);
 }
