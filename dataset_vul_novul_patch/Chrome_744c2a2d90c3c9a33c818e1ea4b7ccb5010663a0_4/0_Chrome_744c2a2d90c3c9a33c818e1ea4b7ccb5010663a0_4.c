   SiteInstanceTest()
       : ui_thread_(BrowserThread::UI, &message_loop_),
        old_client_(NULL),
         old_browser_client_(NULL) {
   }
