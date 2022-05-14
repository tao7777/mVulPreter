  void HistogramsCallback() {
  void HistogramsCallback(base::RepeatingClosure on_done) {
     MockHistogramsCallback();
    on_done.Run();
   }
