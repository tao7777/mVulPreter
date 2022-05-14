  void CreateAndObserveNotifier(
      const std::string& initial_invalidation_state) {
    notifier::NotifierOptions notifier_options;
    notifier_options.request_context_getter =
        new TestURLRequestContextGetter(message_loop_.message_loop_proxy());
    invalidation_notifier_.reset(
        new InvalidationNotifier(
             scoped_ptr<notifier::PushClient>(new notifier::FakePushClient()),
             InvalidationVersionMap(),
             initial_invalidation_state,
            MakeWeakHandle(fake_tracker_.AsWeakPtr()),
             "fake_client_info"));
   }
