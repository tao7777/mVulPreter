 void WebSocketExperimentRunner::DoLoop() {
   if (next_state_ == STATE_NONE) {
     if (task_.get()) {
      AddRef();  // Release in OnTaskCompleted.
       task_->Cancel();
     }
     return;
  }

  State state = next_state_;
  task_state_ = STATE_NONE;
  next_state_ = STATE_NONE;

  switch (state) {
    case STATE_IDLE:
      task_.reset();
      next_state_ = STATE_RUN_WS;
      ChromeThread::PostDelayedTask(
          ChromeThread::IO,
          FROM_HERE,
          NewRunnableMethod(this, &WebSocketExperimentRunner::DoLoop),
          config_.next_delay_ms);
      break;
    case STATE_RUN_WS:
    case STATE_RUN_WSS:
    case STATE_RUN_WS_NODEFAULT_PORT:
    case STATE_RUN_WS_DRAFT75:
    case STATE_RUN_WSS_DRAFT75:
    case STATE_RUN_WS_NODEFAULT_PORT_DRAFT75:
      task_.reset(new WebSocketExperimentTask(
          config_.ws_config[state - STATE_RUN_WS], &task_callback_));
      task_state_ = state;
      if (static_cast<State>(state + 1) == NUM_STATES)
        next_state_ = STATE_IDLE;
      else
        next_state_ = static_cast<State>(state + 1);
      break;
    default:
      NOTREACHED();
      break;
  }
  if (task_.get())
    task_->Run();
 }
