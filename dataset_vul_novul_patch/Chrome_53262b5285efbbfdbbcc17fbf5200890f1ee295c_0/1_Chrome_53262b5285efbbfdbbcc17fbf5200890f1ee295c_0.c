static void DecrementUntilZero(int* count) {
  (*count)--;
  if (!(*count))
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::RunLoop::QuitCurrentWhenIdleClosureDeprecated());
 }
