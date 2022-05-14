RunLoop::RunLoop()
RunLoop::RunLoop(Type type)
     : delegate_(tls_delegate.Get().Get()),
      type_(type),
       origin_task_runner_(ThreadTaskRunnerHandle::Get()),
       weak_factory_(this) {
   DCHECK(delegate_) << "A RunLoop::Delegate must be bound to this thread prior "
                        "to using RunLoop.";
   DCHECK(origin_task_runner_);

  DCHECK(IsNestingAllowedOnCurrentThread() ||
         type_ != Type::kNestableTasksAllowed);
 }
