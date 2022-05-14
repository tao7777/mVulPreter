bool RunLoop::BeforeRun() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

#if DCHECK_IS_ON()
  DCHECK(!run_called_);
  run_called_ = true;
#endif  // DCHECK_IS_ON()

  if (quit_called_)
    return false;

  auto& active_run_loops_ = delegate_->active_run_loops_;
  active_run_loops_.push(this);

  const bool is_nested = active_run_loops_.size() > 1;

  if (is_nested) {
     CHECK(delegate_->allow_nesting_);
     for (auto& observer : delegate_->nesting_observers_)
       observer.OnBeginNestedRunLoop();
    if (type_ == Type::kNestableTasksAllowed)
      delegate_->EnsureWorkScheduled();
   }
 
   running_ = true;
  return true;
}
