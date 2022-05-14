ExtensionTtsController::ExtensionTtsController()
    : ALLOW_THIS_IN_INITIALIZER_LIST(method_factory_(this)),
      current_utterance_(NULL),
      platform_impl_(NULL) {
}
