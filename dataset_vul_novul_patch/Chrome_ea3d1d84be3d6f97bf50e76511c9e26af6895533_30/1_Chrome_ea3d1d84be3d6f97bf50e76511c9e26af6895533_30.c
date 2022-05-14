 void PluginInstance::PluginThreadAsyncCall(void (*func)(void *),
                                            void *user_data) {
  message_loop_->PostTask(FROM_HERE, NewRunnableMethod(
      this, &PluginInstance::OnPluginThreadAsyncCall, func, user_data));
 }
