void RemoveActionCallback(const ActionCallback& callback) {
  DCHECK(g_task_runner.Get());
   DCHECK(g_task_runner.Get()->BelongsToCurrentThread());
   std::vector<ActionCallback>* callbacks = g_callbacks.Pointer();
   for (size_t i = 0; i < callbacks->size(); ++i) {
    if ((*callbacks)[i] == callback) {
       callbacks->erase(callbacks->begin() + i);
       return;
     }
  }
}
