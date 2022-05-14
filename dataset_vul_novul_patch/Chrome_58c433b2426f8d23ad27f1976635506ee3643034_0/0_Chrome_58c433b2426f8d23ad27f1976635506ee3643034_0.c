   QuicConnectionHelperTest()
      : guid_(0),
        framer_(QuicDecrypter::Create(kNULL), QuicEncrypter::Create(kNULL)),
         creator_(guid_, &framer_),
         net_log_(BoundNetLog()),
         scheduler_(new MockScheduler()),
        socket_(&empty_data_, net_log_.net_log()),
        runner_(new TestTaskRunner(&clock_)),
        helper_(new TestConnectionHelper(runner_.get(), &clock_, &socket_)),
        connection_(guid_, IPEndPoint(), helper_),
        frame1_(1, false, 0, data1) {
    connection_.set_visitor(&visitor_);
    connection_.SetScheduler(scheduler_);
  }
