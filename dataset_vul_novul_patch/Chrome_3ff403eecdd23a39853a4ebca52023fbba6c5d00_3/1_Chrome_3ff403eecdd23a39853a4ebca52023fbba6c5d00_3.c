void MessageLoop::SetNestableTasksAllowed(bool allowed) {
  if (allowed) {
     CHECK(RunLoop::IsNestingAllowedOnCurrentThread());
 
     pump_->ScheduleWork();
   }
   nestable_tasks_allowed_ = allowed;
 }
