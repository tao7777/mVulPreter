static void ProxyLocaltimeCallToBrowser(time_t input, struct tm* output,
                                        char* timezone_out,
                                        size_t timezone_out_len) {
  base::Pickle request;
  request.WriteInt(LinuxSandbox::METHOD_LOCALTIME);
   request.WriteString(
       std::string(reinterpret_cast<char*>(&input), sizeof(input)));
 
  memset(output, 0, sizeof(struct tm));

   uint8_t reply_buf[512];
   const ssize_t r = base::UnixDomainSocket::SendRecvMsg(
       GetSandboxFD(), reply_buf, sizeof(reply_buf), NULL, request);
   if (r == -1) {
     return;
   }
 
   base::Pickle reply(reinterpret_cast<char*>(reply_buf), r);
   base::PickleIterator iter(reply);
  if (!ReadTimeStruct(&iter, output, timezone_out, timezone_out_len)) {
     memset(output, 0, sizeof(struct tm));
   }
 }
