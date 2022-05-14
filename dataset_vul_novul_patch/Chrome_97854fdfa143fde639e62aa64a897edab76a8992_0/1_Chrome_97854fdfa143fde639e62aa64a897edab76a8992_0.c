 void WebSocketExperimentRunner::OnTaskCompleted(int result) {
  if (result == net::ERR_ABORTED) {
     task_.reset();
     DLOG(INFO) << "WebSocketExperiment Task is canceled.";
    Release();
    return;
  }
  task_->SaveResult();
  task_.reset();

  DoLoop();
}
