  bool StopInputMethodProcess() {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "StopInputMethodProcess: IBus connection is not alive";
       return false;
    }

    ibus_bus_exit_async(ibus_,
                        FALSE  /* do not restart */,
                        -1  /* timeout */,
                        NULL  /* cancellable */,
                        NULL  /* callback */,
                        NULL  /* user_data */);

    if (ibus_config_) {
      g_object_unref(ibus_config_);
      ibus_config_ = NULL;
    }
     return true;
   }
