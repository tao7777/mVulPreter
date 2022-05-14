void Utterance::FinishAndDestroy() {
  completion_task_->Run();
  completion_task_ = NULL;
  delete this;
}
