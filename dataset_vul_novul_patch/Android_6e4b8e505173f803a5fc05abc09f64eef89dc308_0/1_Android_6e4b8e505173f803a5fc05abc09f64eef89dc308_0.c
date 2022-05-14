 void smp_proc_id_info(tSMP_CB* p_cb, tSMP_INT_DATA* p_data) {
   uint8_t* p = p_data->p_data;
 
   SMP_TRACE_DEBUG("%s", __func__);
   STREAM_TO_ARRAY(p_cb->tk, p, BT_OCTET16_LEN); /* reuse TK for IRK */
   smp_key_distribution_by_transport(p_cb, NULL);
 }
