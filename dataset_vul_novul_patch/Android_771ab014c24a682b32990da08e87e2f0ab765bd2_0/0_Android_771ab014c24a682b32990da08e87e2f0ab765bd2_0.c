 bool FrameworkListener::onDataAvailable(SocketClient *c) {
 char buffer[CMD_BUF_SIZE];
 int len;

    len = TEMP_FAILURE_RETRY(read(c->getSocket(), buffer, sizeof(buffer)));

     if (len < 0) {
         SLOGE("read() failed (%s)", strerror(errno));
         return false;
    } else if (!len) {
         return false;
    } else if (buffer[len-1] != '\0') {
         SLOGW("String is not zero-terminated");
        android_errorWriteLog(0x534e4554, "29831647");
        c->sendMsg(500, "Command too large for buffer", false);
        mSkipToNextNullByte = true;
        return false;
    }
 
     int offset = 0;
     int i;


     for (i = 0; i < len; i++) {
         if (buffer[i] == '\0') {
             /* IMPORTANT: dispatchCommand() expects a zero-terminated string */
            if (mSkipToNextNullByte) {
                mSkipToNextNullByte = false;
            } else {
                dispatchCommand(c, buffer + offset);
            }
             offset = i + 1;
         }
     }
 
    mSkipToNextNullByte = false;
     return true;
 }
