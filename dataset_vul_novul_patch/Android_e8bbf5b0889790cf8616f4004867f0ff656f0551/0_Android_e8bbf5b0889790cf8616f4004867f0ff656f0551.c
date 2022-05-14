void smp_process_keypress_notification(tSMP_CB* p_cb, tSMP_INT_DATA* p_data) {
 uint8_t* p = (uint8_t*)p_data;

   uint8_t reason = SMP_INVALID_PARAMETERS;
 
   SMP_TRACE_DEBUG("%s", __func__);
 
   if (smp_command_has_invalid_parameters(p_cb)) {
    if (p_cb->rcvd_cmd_len < 2) {  // 1 (opcode) + 1 (Notif Type) bytes
      android_errorWriteLog(0x534e4554, "111936834");
    }
     smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
     return;
   }
 
  p_cb->status = *(uint8_t*)p_data;

   if (p != NULL) {
     STREAM_TO_UINT8(p_cb->peer_keypress_notification, p);
   } else {
    p_cb->peer_keypress_notification = BTM_SP_KEY_OUT_OF_RANGE;
 }
  p_cb->cb_evt = SMP_PEER_KEYPR_NOT_EVT;
}
