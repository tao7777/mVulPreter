  void ProcessControlLaunchFailed() {
     ADD_FAILURE();
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::RunLoop::QuitCurrentWhenIdleClosureDeprecated());
   }
