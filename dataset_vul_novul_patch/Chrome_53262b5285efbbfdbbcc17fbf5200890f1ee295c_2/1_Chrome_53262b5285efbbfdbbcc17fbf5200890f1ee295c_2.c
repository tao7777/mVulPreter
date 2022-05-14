  void LaunchServiceProcessControl(bool wait) {
     ServiceProcessControl::GetInstance()->Launch(
        base::Bind(&ServiceProcessControlBrowserTest::ProcessControlLaunched,
                   base::Unretained(this)),
        base::Bind(
             &ServiceProcessControlBrowserTest::ProcessControlLaunchFailed,
            base::Unretained(this)));
    if (wait)
      content::RunMessageLoop();
  }
