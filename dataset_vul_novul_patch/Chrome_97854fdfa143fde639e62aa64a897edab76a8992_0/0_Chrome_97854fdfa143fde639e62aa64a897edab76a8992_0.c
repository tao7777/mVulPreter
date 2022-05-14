 void WebSocketExperimentRunner::OnTaskCompleted(int result) {
  if (next_state_ == STATE_NONE) {
     task_.reset();
     DLOG(INFO) << "WebSocketExperiment Task is canceled.";
    Release();
    return;
  }
  task_->SaveResult();
  task_.reset();

  DoLoop();
}
