 WorkerProcessLauncherTest::WorkerProcessLauncherTest()
    : message_loop_(MessageLoop::TYPE_IO),
      client_pid_(GetCurrentProcessId()),
      permanent_error_(false) {
 }
