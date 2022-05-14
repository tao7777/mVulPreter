 void RunNestedLoopTask(int* counter) {
  RunLoop nested_run_loop;
 
   ThreadTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, BindOnce(&QuitWhenIdleTask, Unretained(&nested_run_loop),
                          Unretained(counter)));

   ThreadTaskRunnerHandle::Get()->PostDelayedTask(
       FROM_HERE, BindOnce(&ShouldNotRunTask), TimeDelta::FromDays(1));
 
  std::unique_ptr<MessageLoop::ScopedNestableTaskAllower> allower;
  if (MessageLoop::current()) {
    allower = base::MakeUnique<MessageLoop::ScopedNestableTaskAllower>(
        MessageLoop::current());
  }
   nested_run_loop.Run();
 
   ++(*counter);
}
