static void sync_lockstate_on_connect(btif_hh_device_t *p_dev)
{
 int keylockstates;

    BTIF_TRACE_EVENT("%s: Syncing keyboard lock states after "\
 "reconnect...",__FUNCTION__);
 /*If the device is connected, update keyboard state */
    update_keyboard_lockstates(p_dev);

 /*Check if the lockstate of caps,scroll,num is set.
     If so, send a report to the kernel
    so the lockstate is in sync */
    keylockstates = get_keylockstates();
 if (keylockstates)
 {

         BTIF_TRACE_DEBUG("%s: Sending hid report to kernel "\
             "indicating lock key state 0x%x",__FUNCTION__,
             keylockstates);
        TEMP_FAILURE_RETRY(usleep(200000));
         toggle_os_keylockstates(p_dev->fd, keylockstates);
     }
     else
 {
        BTIF_TRACE_DEBUG("%s: NOT sending hid report to kernel "\
 "indicating lock key state 0x%x",__FUNCTION__,
            keylockstates);
 }
}
