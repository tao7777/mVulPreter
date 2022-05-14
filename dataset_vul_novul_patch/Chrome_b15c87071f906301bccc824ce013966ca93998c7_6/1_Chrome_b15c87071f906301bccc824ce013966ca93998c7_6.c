WorkerProcessLauncher::Core::Core(
    scoped_refptr<base::SingleThreadTaskRunner> caller_task_runner,
    scoped_ptr<WorkerProcessLauncher::Delegate> launcher_delegate,
    WorkerProcessIpcDelegate* worker_delegate)
     : caller_task_runner_(caller_task_runner),
       launcher_delegate_(launcher_delegate.Pass()),
       worker_delegate_(worker_delegate),
       ipc_enabled_(false),
       launch_backoff_(&kDefaultBackoffPolicy),
       stopping_(false) {
  DCHECK(caller_task_runner_->BelongsToCurrentThread());

  ipc_error_timer_.reset(new base::OneShotTimer<Core>());
  launch_success_timer_.reset(new base::OneShotTimer<Core>());
  launch_timer_.reset(new base::OneShotTimer<Core>());
}
