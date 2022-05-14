   void ShutdownHost() {
    message_loop_.PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::Shutdown, host_,
                              base::Bind(&PostQuitTask, &message_loop_)));
   }
