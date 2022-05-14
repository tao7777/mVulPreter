 TestFlashMessageLoop::TestFlashMessageLoop(TestingInstance* instance)
     : TestCase(instance),
      message_loop_(NULL),
      callback_factory_(this) {
}
