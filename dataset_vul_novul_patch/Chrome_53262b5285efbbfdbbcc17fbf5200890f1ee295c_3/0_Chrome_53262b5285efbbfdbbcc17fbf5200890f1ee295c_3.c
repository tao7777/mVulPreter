  void ProcessControlLaunchFailed() {
  void ProcessControlLaunchFailed(base::OnceClosure on_done) {
     ADD_FAILURE();
    std::move(on_done).Run();
   }
