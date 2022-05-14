 void PluginInstance::PluginThreadAsyncCall(void (*func)(void *),
                                            void *user_data) {
  MessageLoop::current()->PostTask(
      FROM_HERE, NewRunnableMethod(
          this, &PluginInstance::OnPluginThreadAsyncCall, func, user_data));
 }
