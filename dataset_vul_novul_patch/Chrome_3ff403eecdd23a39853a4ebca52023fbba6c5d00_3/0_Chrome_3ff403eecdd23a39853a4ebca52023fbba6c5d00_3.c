void MessageLoop::SetNestableTasksAllowed(bool allowed) {
  if (allowed) {
     CHECK(RunLoop::IsNestingAllowedOnCurrentThread());
 
    // loop that does not go through RunLoop::Run().
     pump_->ScheduleWork();
   }
   nestable_tasks_allowed_ = allowed;
 }
