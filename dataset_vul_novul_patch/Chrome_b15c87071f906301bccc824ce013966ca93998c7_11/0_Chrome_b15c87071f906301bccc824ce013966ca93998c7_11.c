void WorkerProcessLauncherTest::SetUp() {
  task_runner_ = new AutoThreadTaskRunner(
      message_loop_.message_loop_proxy(),
       base::Bind(&WorkerProcessLauncherTest::QuitMainMessageLoop,
                  base::Unretained(this)));
 
   launcher_delegate_.reset(new MockProcessLauncherDelegate());
   EXPECT_CALL(*launcher_delegate_, Send(_))
       .Times(AnyNumber())
       .WillRepeatedly(Return(false));
  EXPECT_CALL(*launcher_delegate_, GetProcessId())
       .Times(AnyNumber())
      .WillRepeatedly(ReturnPointee(&client_pid_));
  EXPECT_CALL(*launcher_delegate_, IsPermanentError(_))
      .Times(AnyNumber())
      .WillRepeatedly(ReturnPointee(&permanent_error_));
   EXPECT_CALL(*launcher_delegate_, KillProcess(_))
       .Times(AnyNumber())
       .WillRepeatedly(Invoke(this, &WorkerProcessLauncherTest::KillProcess));

  EXPECT_CALL(ipc_delegate_, OnMessageReceived(_))
      .Times(AnyNumber())
      .WillRepeatedly(Return(false));
}
