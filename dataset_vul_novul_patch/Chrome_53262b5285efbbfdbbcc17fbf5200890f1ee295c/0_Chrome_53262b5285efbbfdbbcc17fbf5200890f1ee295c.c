  static void CloudPrintInfoCallback(bool enabled,
  void LaunchServiceProcessControlAndWait() {
    base::RunLoop run_loop;
    LaunchServiceProcessControl(run_loop.QuitClosure());
    run_loop.Run();
   }
