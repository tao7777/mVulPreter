void FrameworkListener::init(const char *socketName UNUSED, bool withSeq) {
    mCommands = new FrameworkCommandCollection();

     errorRate = 0;
     mCommandCount = 0;
     mWithSeq = withSeq;
 }
