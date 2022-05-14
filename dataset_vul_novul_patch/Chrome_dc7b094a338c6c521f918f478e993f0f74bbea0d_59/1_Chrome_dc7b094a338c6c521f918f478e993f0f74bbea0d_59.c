  static InputMethodStatusConnection* GetInstance() {
    return Singleton<InputMethodStatusConnection,
        LeakySingletonTraits<InputMethodStatusConnection> >::get();
   }
