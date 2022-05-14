static void toggle_os_keylockstates(int fd, int changedlockstates)
{
    BTIF_TRACE_EVENT("%s: fd = %d, changedlockstates = 0x%x",
        __FUNCTION__, fd, changedlockstates);
    UINT8 hidreport[9];
 int reportIndex;
    memset(hidreport,0,9);
    hidreport[0]=1;
    reportIndex=4;

 if (changedlockstates & BTIF_HH_KEYSTATE_MASK_CAPSLOCK) {
        BTIF_TRACE_DEBUG("%s Setting CAPSLOCK", __FUNCTION__);
        hidreport[reportIndex++] = (UINT8)HID_REPORT_CAPSLOCK;
 }

 if (changedlockstates & BTIF_HH_KEYSTATE_MASK_NUMLOCK) {
        BTIF_TRACE_DEBUG("%s Setting NUMLOCK", __FUNCTION__);
        hidreport[reportIndex++] = (UINT8)HID_REPORT_NUMLOCK;
 }

 if (changedlockstates & BTIF_HH_KEYSTATE_MASK_SCROLLLOCK) {
        BTIF_TRACE_DEBUG("%s Setting SCROLLLOCK", __FUNCTION__);
        hidreport[reportIndex++] = (UINT8) HID_REPORT_SCROLLLOCK;
 }

     BTIF_TRACE_DEBUG("Writing hidreport #1 to os: "\
 "%s:  %x %x %x", __FUNCTION__,
         hidreport[0], hidreport[1], hidreport[2]);
    BTIF_TRACE_DEBUG("%s:  %x %x %x", __FUNCTION__,
         hidreport[3], hidreport[4], hidreport[5]);

     BTIF_TRACE_DEBUG("%s:  %x %x %x", __FUNCTION__,
          hidreport[6], hidreport[7], hidreport[8]);
     bta_hh_co_write(fd , hidreport, sizeof(hidreport));
    TEMP_FAILURE_RETRY(usleep(200000));
     memset(hidreport,0,9);
     hidreport[0]=1;
     BTIF_TRACE_DEBUG("Writing hidreport #2 to os: "\
 "%s:  %x %x %x", __FUNCTION__,
         hidreport[0], hidreport[1], hidreport[2]);
    BTIF_TRACE_DEBUG("%s:  %x %x %x", __FUNCTION__,
         hidreport[3], hidreport[4], hidreport[5]);
    BTIF_TRACE_DEBUG("%s:  %x %x %x ", __FUNCTION__,
         hidreport[6], hidreport[7], hidreport[8]);
    bta_hh_co_write(fd , hidreport, sizeof(hidreport));
}
