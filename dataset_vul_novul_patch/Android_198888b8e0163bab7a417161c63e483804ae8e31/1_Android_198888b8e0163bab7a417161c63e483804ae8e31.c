void smp_proc_master_id(tSMP_CB* p_cb, tSMP_INT_DATA* p_data) {
 uint8_t* p = p_data->p_data;

   tBTM_LE_PENC_KEYS le_key;
 
   SMP_TRACE_DEBUG("%s", __func__);
   smp_update_key_mask(p_cb, SMP_SEC_KEY_TYPE_ENC, true);
 
   STREAM_TO_UINT16(le_key.ediv, p);
  STREAM_TO_ARRAY(le_key.rand, p, BT_OCTET8_LEN);

 /* store the encryption keys from peer device */
  memcpy(le_key.ltk, p_cb->ltk, BT_OCTET16_LEN);
  le_key.sec_level = p_cb->sec_level;
  le_key.key_size = p_cb->loc_enc_size;

 if ((p_cb->peer_auth_req & SMP_AUTH_BOND) &&
 (p_cb->loc_auth_req & SMP_AUTH_BOND))
    btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_PENC,
 (tBTM_LE_KEY_VALUE*)&le_key, true);

  smp_key_distribution(p_cb, NULL);
}
