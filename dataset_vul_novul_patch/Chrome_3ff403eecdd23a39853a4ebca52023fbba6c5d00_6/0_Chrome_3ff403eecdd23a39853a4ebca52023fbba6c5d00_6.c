 void RunNestedLoopTask(int* counter) {
  RunLoop nested_run_loop(RunLoop::Type::kNestableTasksAllowed);
 
   ThreadTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, BindOnce(&QuitWhenIdleTask, Unretained(&nested_run_loop),
                          Unretained(counter)));

   ThreadTaskRunnerHandle::Get()->PostDelayedTask(
       FROM_HERE, BindOnce(&ShouldNotRunTask), TimeDelta::FromDays(1));
 
   nested_run_loop.Run();
 
   ++(*counter);
}
