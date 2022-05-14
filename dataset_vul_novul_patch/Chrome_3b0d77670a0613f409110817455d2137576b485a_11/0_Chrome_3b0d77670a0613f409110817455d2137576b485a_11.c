 PP_Bool LaunchSelLdr(PP_Instance instance,
                     const char* alleged_url, int socket_count,
                      void* imc_handles) {
   std::vector<nacl::FileDescriptor> sockets;
   IPC::Sender* sender = content::RenderThread::Get();
   if (sender == NULL)
     sender = g_background_thread_sender.Pointer()->get();
 
   if (!sender->Send(new ChromeViewHostMsg_LaunchNaCl(
          GURL(alleged_url), socket_count, &sockets)))
     return PP_FALSE;
 
   CHECK(static_cast<int>(sockets.size()) == socket_count);
   for (int i = 0; i < socket_count; i++) {
    static_cast<nacl::Handle*>(imc_handles)[i] =
        nacl::ToNativeHandle(sockets[i]);
  }

   return PP_TRUE;
 }
