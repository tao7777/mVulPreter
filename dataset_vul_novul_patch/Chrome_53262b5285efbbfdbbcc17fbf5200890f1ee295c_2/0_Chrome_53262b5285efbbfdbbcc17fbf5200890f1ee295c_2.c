  void LaunchServiceProcessControl(bool wait) {
  void LaunchServiceProcessControl(base::RepeatingClosure on_launched) {
     ServiceProcessControl::GetInstance()->Launch(
        base::BindOnce(
            &ServiceProcessControlBrowserTest::ProcessControlLaunched,
            base::Unretained(this), on_launched),
        base::BindOnce(
             &ServiceProcessControlBrowserTest::ProcessControlLaunchFailed,
