 TestFlashMessageLoop::~TestFlashMessageLoop() {
   PP_DCHECK(!message_loop_);

  ResetTestObject();
  if (instance_so_)
    instance_so_->clear_owner();
 }
