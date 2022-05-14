void wifi_cleanup(wifi_handle handle, wifi_cleaned_up_handler handler)
{
    hal_info *info = getHalInfo(handle);
 char buf[64];

    info->cleaned_up_handler = handler;
 if (write(info->cleanup_socks[0], "Exit", 4) < 1) {
        ALOGE("could not write to the cleanup socket");
 } else {
        memset(buf, 0, sizeof(buf));
 int result = read(info->cleanup_socks[0], buf, sizeof(buf));
        ALOGE("%s: Read after POLL returned %d, error no = %d", __FUNCTION__, result, errno);
 if (strncmp(buf, "Done", 4) == 0) {
            ALOGE("Event processing terminated");
 } else {
            ALOGD("Rx'ed %s", buf);
 }
 }
    info->clean_up = true;
    pthread_mutex_lock(&info->cb_lock);

 int bad_commands = 0;

 for (int i = 0; i < info->num_event_cb; i++) {
        cb_info *cbi = &(info->event_cb[i]);
 WifiCommand *cmd = (WifiCommand *)cbi->cb_arg;
        ALOGI("Command left in event_cb %p:%s", cmd, (cmd ? cmd->getType(): ""));
 }

 while (info->num_cmd > bad_commands) {
 int num_cmd = info->num_cmd;
        cmd_info *cmdi = &(info->cmd[bad_commands]);
 WifiCommand *cmd = cmdi->cmd;
 if (cmd != NULL) {
            ALOGI("Cancelling command %p:%s", cmd, cmd->getType());

             pthread_mutex_unlock(&info->cb_lock);
             cmd->cancel();
             pthread_mutex_lock(&info->cb_lock);
             if (num_cmd == info->num_cmd) {
                 ALOGI("Cancelling command %p:%s did not work", cmd, (cmd ? cmd->getType(): ""));
                 bad_commands++;
             }
            /* release reference added when command is saved */
            cmd->releaseRef();
         }
     }
 
 for (int i = 0; i < info->num_event_cb; i++) {
        cb_info *cbi = &(info->event_cb[i]);
 WifiCommand *cmd = (WifiCommand *)cbi->cb_arg;
        ALOGE("Leaked command %p", cmd);
 }
    pthread_mutex_unlock(&info->cb_lock);
    internal_cleaned_up_handler(handle);
}
