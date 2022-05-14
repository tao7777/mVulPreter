void WorkerProcessLauncherTest::SetUp() {
  task_runner_ = new AutoThreadTaskRunner(
      message_loop_.message_loop_proxy(),
       base::Bind(&WorkerProcessLauncherTest::QuitMainMessageLoop,
                  base::Unretained(this)));
 
  exit_code_ = STILL_ACTIVE;
   launcher_delegate_.reset(new MockProcessLauncherDelegate());
   EXPECT_CALL(*launcher_delegate_, Send(_))
       .Times(AnyNumber())
       .WillRepeatedly(Return(false));
  EXPECT_CALL(*launcher_delegate_, GetExitCode())
       .Times(AnyNumber())
      .WillRepeatedly(ReturnPointee(&exit_code_));
   EXPECT_CALL(*launcher_delegate_, KillProcess(_))
       .Times(AnyNumber())
       .WillRepeatedly(Invoke(this, &WorkerProcessLauncherTest::KillProcess));

  EXPECT_CALL(ipc_delegate_, OnMessageReceived(_))
      .Times(AnyNumber())
      .WillRepeatedly(Return(false));
}
