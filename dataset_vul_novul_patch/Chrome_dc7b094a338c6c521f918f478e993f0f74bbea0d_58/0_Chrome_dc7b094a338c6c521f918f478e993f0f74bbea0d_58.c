  static InputMethodStatusConnection* GetConnection(
  // TODO(satorux,yusukes): Remove use of singleton here.
  static IBusControllerImpl* GetInstance() {
    return Singleton<IBusControllerImpl,
        LeakySingletonTraits<IBusControllerImpl> >::get();
   }
