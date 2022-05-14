bt_status_t btif_storage_add_bonded_device(bt_bdaddr_t *remote_bd_addr,
                                           LINK_KEY link_key,
 uint8_t key_type,
 uint8_t pin_length)
{
 bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

     int ret = btif_config_set_int(bdstr, "LinkKeyType", (int)key_type);
     ret &= btif_config_set_int(bdstr, "PinLength", (int)pin_length);
     ret &= btif_config_set_bin(bdstr, "LinkKey", link_key, sizeof(LINK_KEY));
     /* write bonded info immediately */
     btif_config_flush();
     return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}
