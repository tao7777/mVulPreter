  bool Init(const IPC::ChannelHandle& channel_handle,
            PP_Module pp_module,
            PP_GetInterface_Func local_get_interface,
            const ppapi::Preferences& preferences,
            SyncMessageStatusReceiver* status_receiver) {
    dispatcher_delegate_.reset(new ProxyChannelDelegate);
    dispatcher_.reset(new ppapi::proxy::HostDispatcher(
        pp_module, local_get_interface, status_receiver));
    if (!dispatcher_->InitHostWithChannel(dispatcher_delegate_.get(),
                                          channel_handle,
                                          true,  // Client.
                                          preferences)) {
      dispatcher_.reset();
      dispatcher_delegate_.reset();
      return false;
    }
    return true;
  }
