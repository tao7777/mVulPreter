void SandboxIPCHandler::HandleLocaltime(
     int fd,
     base::PickleIterator iter,
     const std::vector<base::ScopedFD>& fds) {
 
   std::string time_string;
   if (!iter.ReadString(&time_string) || time_string.size() != sizeof(time_t))
     return;
 
   time_t time;
   memcpy(&time, time_string.data(), sizeof(time));
   const struct tm* expanded_time = localtime(&time);
 
  std::string result_string;
  const char* time_zone_string = "";
   if (expanded_time) {
    result_string = std::string(reinterpret_cast<const char*>(expanded_time),
                                sizeof(struct tm));
    time_zone_string = expanded_time->tm_zone;
   }
  base::Pickle reply;
  reply.WriteString(result_string);
  reply.WriteString(time_zone_string);
   SendRendererReply(fds, reply, -1);
 }
