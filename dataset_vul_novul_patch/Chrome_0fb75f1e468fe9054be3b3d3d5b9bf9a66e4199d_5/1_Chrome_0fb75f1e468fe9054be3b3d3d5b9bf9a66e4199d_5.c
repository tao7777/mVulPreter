TargetThread::TargetThread()
     : thread_started_event_(false, false), finish_event_(false, false),
      id_(0) {}
