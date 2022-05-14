 void smp_proc_id_info(tSMP_CB* p_cb, tSMP_INT_DATA* p_data) {
   uint8_t* p = p_data->p_data;
 
   SMP_TRACE_DEBUG("%s", __func__);

  if (smp_command_has_invalid_parameters(p_cb)) {
    tSMP_INT_DATA smp_int_data;
    smp_int_data.status = SMP_INVALID_PARAMETERS;
    android_errorWriteLog(0x534e4554, "111937065");
    smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &smp_int_data);
    return;
  }

   STREAM_TO_ARRAY(p_cb->tk, p, BT_OCTET16_LEN); /* reuse TK for IRK */
   smp_key_distribution_by_transport(p_cb, NULL);
 }
