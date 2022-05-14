bt_status_t btif_dm_pin_reply( const bt_bdaddr_t *bd_addr, uint8_t accept,

                                uint8_t pin_len, bt_pin_code_t *pin_code)
 {
     BTIF_TRACE_EVENT("%s: accept=%d", __FUNCTION__, accept);
    if (pin_code == NULL || pin_len > PIN_CODE_LEN)
         return BT_STATUS_FAIL;
 #if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
 
 if (pairing_cb.is_le_only)
 {
 int i;
        UINT32 passkey = 0;
 int multi[] = {100000, 10000, 1000, 100, 10,1};
        BD_ADDR remote_bd_addr;
        bdcpy(remote_bd_addr, bd_addr->address);
 for (i = 0; i < 6; i++)
 {
            passkey += (multi[i] * (pin_code->pin[i] - '0'));
 }
        BTIF_TRACE_DEBUG("btif_dm_pin_reply: passkey: %d", passkey);
        BTA_DmBlePasskeyReply(remote_bd_addr, accept, passkey);

 }
 else
 {
        BTA_DmPinReply( (UINT8 *)bd_addr->address, accept, pin_len, pin_code->pin);
 if (accept)
            pairing_cb.pin_code_len = pin_len;
 }
#else
    BTA_DmPinReply( (UINT8 *)bd_addr->address, accept, pin_len, pin_code->pin);

 if (accept)
        pairing_cb.pin_code_len = pin_len;
#endif
 return BT_STATUS_SUCCESS;
}
