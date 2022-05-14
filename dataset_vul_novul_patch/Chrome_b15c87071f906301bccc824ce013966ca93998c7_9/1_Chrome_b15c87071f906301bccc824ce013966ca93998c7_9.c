 void WorkerProcessLauncherTest::KillProcess(DWORD exit_code) {
  exit_code_ = exit_code;
   BOOL result = SetEvent(process_exit_event_);
   EXPECT_TRUE(result);
 }
