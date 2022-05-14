static void btif_dm_upstreams_evt(UINT16 event, char* p_param)
{
    tBTA_DM_SEC *p_data = (tBTA_DM_SEC*)p_param;
    tBTA_SERVICE_MASK service_mask;
 uint32_t i;
 bt_bdaddr_t bd_addr;

    BTIF_TRACE_EVENT("btif_dm_upstreams_cback  ev: %s", dump_dm_event(event));

 switch (event)
 {
 case BTA_DM_ENABLE_EVT:
 {
             BD_NAME bdname;
 bt_status_t status;
 bt_property_t prop;
             prop.type = BT_PROPERTY_BDNAME;
             prop.len = BD_NAME_LEN;
             prop.val = (void*)bdname;

             status = btif_storage_get_adapter_property(&prop);
 if (status == BT_STATUS_SUCCESS)
 {
 /* A name exists in the storage. Make this the device name */
                 BTA_DmSetDeviceName((char*)prop.val);
 }
 else
 {
 /* Storage does not have a name yet.
                  * Use the default name and write it to the chip
                  */
                 BTA_DmSetDeviceName(btif_get_default_local_name());
 }

#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
 /* Enable local privacy */
             BTA_DmBleConfigLocalPrivacy(BLE_LOCAL_PRIVACY_ENABLED);
#endif

 /* for each of the enabled services in the mask, trigger the profile
              * enable */
             service_mask = btif_get_enabled_services_mask();
 for (i=0; i <= BTA_MAX_SERVICE_ID; i++)
 {
 if (service_mask &
 (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i)))
 {
                     btif_in_execute_service_request(i, TRUE);
 }
 }
 /* clear control blocks */
             memset(&pairing_cb, 0, sizeof(btif_dm_pairing_cb_t));
             pairing_cb.bond_type = BOND_TYPE_PERSISTENT;

 /* This function will also trigger the adapter_properties_cb
             ** and bonded_devices_info_cb
             */
             btif_storage_load_bonded_devices();

             btif_storage_load_autopair_device_list();

             btif_enable_bluetooth_evt(p_data->enable.status);
 }
 break;

 case BTA_DM_DISABLE_EVT:
 /* for each of the enabled services in the mask, trigger the profile
             * disable */
            service_mask = btif_get_enabled_services_mask();
 for (i=0; i <= BTA_MAX_SERVICE_ID; i++)
 {
 if (service_mask &
 (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i)))
 {
                    btif_in_execute_service_request(i, FALSE);
 }
 }
            btif_disable_bluetooth_evt();
 break;

 case BTA_DM_PIN_REQ_EVT:
            btif_dm_pin_req_evt(&p_data->pin_req);
 break;

 case BTA_DM_AUTH_CMPL_EVT:
            btif_dm_auth_cmpl_evt(&p_data->auth_cmpl);
 break;

 case BTA_DM_BOND_CANCEL_CMPL_EVT:
 if (pairing_cb.state == BT_BOND_STATE_BONDING)
 {
                bdcpy(bd_addr.address, pairing_cb.bd_addr);
                btm_set_bond_type_dev(pairing_cb.bd_addr, BOND_TYPE_UNKNOWN);
                bond_state_changed(p_data->bond_cancel_cmpl.result, &bd_addr, BT_BOND_STATE_NONE);
 }
 break;

 case BTA_DM_SP_CFM_REQ_EVT:
            btif_dm_ssp_cfm_req_evt(&p_data->cfm_req);
 break;
 case BTA_DM_SP_KEY_NOTIF_EVT:
            btif_dm_ssp_key_notif_evt(&p_data->key_notif);
 break;

 case BTA_DM_DEV_UNPAIRED_EVT:
            bdcpy(bd_addr.address, p_data->link_down.bd_addr);
            btm_set_bond_type_dev(p_data->link_down.bd_addr, BOND_TYPE_UNKNOWN);

 /*special handling for HID devices */
 #if (defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE))
            btif_hh_remove_device(bd_addr);
 #endif
            btif_storage_remove_bonded_device(&bd_addr);
            bond_state_changed(BT_STATUS_SUCCESS, &bd_addr, BT_BOND_STATE_NONE);
 break;

 case BTA_DM_BUSY_LEVEL_EVT:
 {

 if (p_data->busy_level.level_flags & BTM_BL_INQUIRY_PAGING_MASK)
 {
 if (p_data->busy_level.level_flags == BTM_BL_INQUIRY_STARTED)
 {
                       HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb,
                                                BT_DISCOVERY_STARTED);
                       btif_dm_inquiry_in_progress = TRUE;
 }
 else if (p_data->busy_level.level_flags == BTM_BL_INQUIRY_CANCELLED)
 {
                       HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb,
                                                BT_DISCOVERY_STOPPED);
                       btif_dm_inquiry_in_progress = FALSE;
 }
 else if (p_data->busy_level.level_flags == BTM_BL_INQUIRY_COMPLETE)
 {
                       btif_dm_inquiry_in_progress = FALSE;
 }
 }
 }break;

 case BTA_DM_LINK_UP_EVT:
            bdcpy(bd_addr.address, p_data->link_up.bd_addr);
            BTIF_TRACE_DEBUG("BTA_DM_LINK_UP_EVT. Sending BT_ACL_STATE_CONNECTED");

            btif_update_remote_version_property(&bd_addr);

            HAL_CBACK(bt_hal_cbacks, acl_state_changed_cb, BT_STATUS_SUCCESS,
 &bd_addr, BT_ACL_STATE_CONNECTED);
 break;

 case BTA_DM_LINK_DOWN_EVT:
            bdcpy(bd_addr.address, p_data->link_down.bd_addr);
            btm_set_bond_type_dev(p_data->link_down.bd_addr, BOND_TYPE_UNKNOWN);
            BTIF_TRACE_DEBUG("BTA_DM_LINK_DOWN_EVT. Sending BT_ACL_STATE_DISCONNECTED");
            HAL_CBACK(bt_hal_cbacks, acl_state_changed_cb, BT_STATUS_SUCCESS,
 &bd_addr, BT_ACL_STATE_DISCONNECTED);
 break;

 case BTA_DM_HW_ERROR_EVT:

             BTIF_TRACE_ERROR("Received H/W Error. ");
             /* Flush storage data */
             btif_config_flush();
            usleep(100000); /* 100milliseconds */
             /* Killing the process to force a restart as part of fault tolerance */
             kill(getpid(), SIGKILL);
             break;

#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
 case BTA_DM_BLE_KEY_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_KEY_EVT key_type=0x%02x ", p_data->ble_key.key_type);

 /* If this pairing is by-product of local initiated GATT client Read or Write,
            BTA would not have sent BTA_DM_BLE_SEC_REQ_EVT event and Bond state would not
            have setup properly. Setup pairing_cb and notify App about Bonding state now*/
 if (pairing_cb.state != BT_BOND_STATE_BONDING)
 {
                BTIF_TRACE_DEBUG("Bond state not sent to App so far.Notify the app now");
                bond_state_changed(BT_STATUS_SUCCESS, (bt_bdaddr_t*)p_data->ble_key.bd_addr,
                                   BT_BOND_STATE_BONDING);
 }
 else if (memcmp (pairing_cb.bd_addr, p_data->ble_key.bd_addr, BD_ADDR_LEN)!=0)
 {
                BTIF_TRACE_ERROR("BD mismatch discard BLE key_type=%d ",p_data->ble_key.key_type);
 break;
 }

 switch (p_data->ble_key.key_type)
 {
 case BTA_LE_KEY_PENC:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_PENC");
                    pairing_cb.ble.is_penc_key_rcvd = TRUE;
                    pairing_cb.ble.penc_key = p_data->ble_key.p_key_value->penc_key;
 break;

 case BTA_LE_KEY_PID:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_PID");
                    pairing_cb.ble.is_pid_key_rcvd = TRUE;
                    pairing_cb.ble.pid_key = p_data->ble_key.p_key_value->pid_key;
 break;

 case BTA_LE_KEY_PCSRK:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_PCSRK");
                    pairing_cb.ble.is_pcsrk_key_rcvd = TRUE;
                    pairing_cb.ble.pcsrk_key = p_data->ble_key.p_key_value->pcsrk_key;
 break;

 case BTA_LE_KEY_LENC:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_LENC");
                    pairing_cb.ble.is_lenc_key_rcvd = TRUE;
                    pairing_cb.ble.lenc_key = p_data->ble_key.p_key_value->lenc_key;
 break;

 case BTA_LE_KEY_LCSRK:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_LCSRK");
                    pairing_cb.ble.is_lcsrk_key_rcvd = TRUE;
                    pairing_cb.ble.lcsrk_key = p_data->ble_key.p_key_value->lcsrk_key;
 break;

 case BTA_LE_KEY_LID:
                    BTIF_TRACE_DEBUG("Rcv BTA_LE_KEY_LID");
                    pairing_cb.ble.is_lidk_key_rcvd =  TRUE;
 break;

 default:
                    BTIF_TRACE_ERROR("unknown BLE key type (0x%02x)", p_data->ble_key.key_type);
 break;
 }
 break;
 case BTA_DM_BLE_SEC_REQ_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_SEC_REQ_EVT. ");
            btif_dm_ble_sec_req_evt(&p_data->ble_req);
 break;
 case BTA_DM_BLE_PASSKEY_NOTIF_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_PASSKEY_NOTIF_EVT. ");
            btif_dm_ble_key_notif_evt(&p_data->key_notif);
 break;
 case BTA_DM_BLE_PASSKEY_REQ_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_PASSKEY_REQ_EVT. ");
            btif_dm_ble_passkey_req_evt(&p_data->pin_req);
 break;
 case BTA_DM_BLE_NC_REQ_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_PASSKEY_REQ_EVT. ");
            btif_dm_ble_key_nc_req_evt(&p_data->key_notif);
 break;
 case BTA_DM_BLE_OOB_REQ_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_OOB_REQ_EVT. ");
 break;
 case BTA_DM_BLE_LOCAL_IR_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_LOCAL_IR_EVT. ");
            ble_local_key_cb.is_id_keys_rcvd = TRUE;
            memcpy(&ble_local_key_cb.id_keys.irk[0],
 &p_data->ble_id_keys.irk[0], sizeof(BT_OCTET16));
            memcpy(&ble_local_key_cb.id_keys.ir[0],
 &p_data->ble_id_keys.ir[0], sizeof(BT_OCTET16));
            memcpy(&ble_local_key_cb.id_keys.dhk[0],
 &p_data->ble_id_keys.dhk[0], sizeof(BT_OCTET16));
            btif_storage_add_ble_local_key( (char *)&ble_local_key_cb.id_keys.irk[0],
                                            BTIF_DM_LE_LOCAL_KEY_IRK,
                                            BT_OCTET16_LEN);
            btif_storage_add_ble_local_key( (char *)&ble_local_key_cb.id_keys.ir[0],
                                            BTIF_DM_LE_LOCAL_KEY_IR,
                                            BT_OCTET16_LEN);
            btif_storage_add_ble_local_key( (char *)&ble_local_key_cb.id_keys.dhk[0],
                                            BTIF_DM_LE_LOCAL_KEY_DHK,
                                            BT_OCTET16_LEN);
 break;
 case BTA_DM_BLE_LOCAL_ER_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_LOCAL_ER_EVT. ");
            ble_local_key_cb.is_er_rcvd = TRUE;
            memcpy(&ble_local_key_cb.er[0], &p_data->ble_er[0], sizeof(BT_OCTET16));
            btif_storage_add_ble_local_key( (char *)&ble_local_key_cb.er[0],
                                            BTIF_DM_LE_LOCAL_KEY_ER,
                                            BT_OCTET16_LEN);
 break;

 case BTA_DM_BLE_AUTH_CMPL_EVT:
            BTIF_TRACE_DEBUG("BTA_DM_BLE_AUTH_CMPL_EVT. ");
            btif_dm_ble_auth_cmpl_evt(&p_data->auth_cmpl);
 break;

 case BTA_DM_LE_FEATURES_READ:
 {
            tBTM_BLE_VSC_CB cmn_vsc_cb;
 bt_local_le_features_t local_le_features;
 char buf[512];
 bt_property_t prop;
            prop.type = BT_PROPERTY_LOCAL_LE_FEATURES;
            prop.val = (void*)buf;
            prop.len = sizeof(buf);

 /* LE features are not stored in storage. Should be retrived from stack */
            BTM_BleGetVendorCapabilities(&cmn_vsc_cb);
            local_le_features.local_privacy_enabled = BTM_BleLocalPrivacyEnabled();

            prop.len = sizeof (bt_local_le_features_t);
 if (cmn_vsc_cb.filter_support == 1)
                local_le_features.max_adv_filter_supported = cmn_vsc_cb.max_filter;
 else
                local_le_features.max_adv_filter_supported = 0;
            local_le_features.max_adv_instance = cmn_vsc_cb.adv_inst_max;
            local_le_features.max_irk_list_size = cmn_vsc_cb.max_irk_list_sz;
            local_le_features.rpa_offload_supported = cmn_vsc_cb.rpa_offloading;
            local_le_features.activity_energy_info_supported = cmn_vsc_cb.energy_support;
            local_le_features.scan_result_storage_size = cmn_vsc_cb.tot_scan_results_strg;
            local_le_features.version_supported = cmn_vsc_cb.version_supported;
            local_le_features.total_trackable_advertisers =
                        cmn_vsc_cb.total_trackable_advertisers;

            local_le_features.extended_scan_support = cmn_vsc_cb.extended_scan_support > 0;
            local_le_features.debug_logging_supported = cmn_vsc_cb.debug_logging_supported > 0;

            memcpy(prop.val, &local_le_features, prop.len);
            HAL_CBACK(bt_hal_cbacks, adapter_properties_cb, BT_STATUS_SUCCESS, 1, &prop);
 break;
 }

 case BTA_DM_ENER_INFO_READ:
 {
 btif_activity_energy_info_cb_t *p_ener_data = (btif_activity_energy_info_cb_t*) p_param;
            bt_activity_energy_info energy_info;
            energy_info.status = p_ener_data->status;
            energy_info.ctrl_state = p_ener_data->ctrl_state;
            energy_info.rx_time = p_ener_data->rx_time;
            energy_info.tx_time = p_ener_data->tx_time;
            energy_info.idle_time = p_ener_data->idle_time;
            energy_info.energy_used = p_ener_data->energy_used;
            HAL_CBACK(bt_hal_cbacks, energy_info_cb, &energy_info);
 break;
 }
#endif

 case BTA_DM_AUTHORIZE_EVT:
 case BTA_DM_SIG_STRENGTH_EVT:
 case BTA_DM_SP_RMT_OOB_EVT:
 case BTA_DM_SP_KEYPRESS_EVT:
 case BTA_DM_ROLE_CHG_EVT:

 default:
            BTIF_TRACE_WARNING( "btif_dm_cback : unhandled event (%d)", event );
 break;
 }

    btif_dm_data_free(event, p_data);
}
