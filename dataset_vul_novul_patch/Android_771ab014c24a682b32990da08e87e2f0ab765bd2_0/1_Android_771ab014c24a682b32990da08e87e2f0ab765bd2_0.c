 bool FrameworkListener::onDataAvailable(SocketClient *c) {
 char buffer[CMD_BUF_SIZE];
 int len;

    len = TEMP_FAILURE_RETRY(read(c->getSocket(), buffer, sizeof(buffer)));

     if (len < 0) {
         SLOGE("read() failed (%s)", strerror(errno));
         return false;
    } else if (!len)
         return false;
   if(buffer[len-1] != '\0')
         SLOGW("String is not zero-terminated");
 
     int offset = 0;
     int i;


     for (i = 0; i < len; i++) {
         if (buffer[i] == '\0') {
             /* IMPORTANT: dispatchCommand() expects a zero-terminated string */
            dispatchCommand(c, buffer + offset);
             offset = i + 1;
         }
     }
 
     return true;
 }
