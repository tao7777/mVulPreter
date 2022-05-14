 bool MessageLoop::NestableTasksAllowed() const {
  return nestable_tasks_allowed_ || run_loop_client_->ProcessingTasksAllowed();
 }
