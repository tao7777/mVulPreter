void bta_hh_co_open(UINT8 dev_handle, UINT8 sub_class, tBTA_HH_ATTR_MASK attr_mask,
                    UINT8 app_id)
{
    UINT32 i;
 btif_hh_device_t *p_dev = NULL;

 if (dev_handle == BTA_HH_INVALID_HANDLE) {
        APPL_TRACE_WARNING("%s: Oops, dev_handle (%d) is invalid...",
                           __FUNCTION__, dev_handle);
 return;
 }

 for (i = 0; i < BTIF_HH_MAX_HID; i++) {
        p_dev = &btif_hh_cb.devices[i];
 if (p_dev->dev_status != BTHH_CONN_STATE_UNKNOWN &&
            p_dev->dev_handle == dev_handle) {
            APPL_TRACE_WARNING("%s: Found an existing device with the same handle "
 "dev_status = %d",__FUNCTION__,
                                                                p_dev->dev_status);
            APPL_TRACE_WARNING("%s:     bd_addr = [%02X:%02X:%02X:%02X:%02X:]", __FUNCTION__,
                 p_dev->bd_addr.address[0], p_dev->bd_addr.address[1], p_dev->bd_addr.address[2],
                 p_dev->bd_addr.address[3], p_dev->bd_addr.address[4]);
                 APPL_TRACE_WARNING("%s:     attr_mask = 0x%04x, sub_class = 0x%02x, app_id = %d",

                                   __FUNCTION__, p_dev->attr_mask, p_dev->sub_class, p_dev->app_id);
 
             if(p_dev->fd<0) {
                p_dev->fd = open(dev_path, O_RDWR | O_CLOEXEC);
                 if (p_dev->fd < 0){
                     APPL_TRACE_ERROR("%s: Error: failed to open uhid, err:%s",
                                                                     __FUNCTION__,strerror(errno));
 return;
 }else
                    APPL_TRACE_DEBUG("%s: uhid fd = %d", __FUNCTION__, p_dev->fd);
 }

            p_dev->hh_keep_polling = 1;
            p_dev->hh_poll_thread_id = create_thread(btif_hh_poll_event_thread, p_dev);
 break;
 }
        p_dev = NULL;
 }

 if (p_dev == NULL) {
 for (i = 0; i < BTIF_HH_MAX_HID; i++) {
 if (btif_hh_cb.devices[i].dev_status == BTHH_CONN_STATE_UNKNOWN) {
                p_dev = &btif_hh_cb.devices[i];
                p_dev->dev_handle = dev_handle;
                p_dev->attr_mask  = attr_mask;
                p_dev->sub_class  = sub_class;
                p_dev->app_id     = app_id;
                p_dev->local_vup  = FALSE;

 
                 btif_hh_cb.device_num++;
                p_dev->fd = open(dev_path, O_RDWR | O_CLOEXEC);
                 if (p_dev->fd < 0){
                     APPL_TRACE_ERROR("%s: Error: failed to open uhid, err:%s",
                                                                     __FUNCTION__,strerror(errno));
 return;
 }else{
                    APPL_TRACE_DEBUG("%s: uhid fd = %d", __FUNCTION__, p_dev->fd);
                    p_dev->hh_keep_polling = 1;
                    p_dev->hh_poll_thread_id = create_thread(btif_hh_poll_event_thread, p_dev);
 }


 break;
 }
 }
 }

 if (p_dev == NULL) {
        APPL_TRACE_ERROR("%s: Error: too many HID devices are connected", __FUNCTION__);
 return;
 }

    p_dev->dev_status = BTHH_CONN_STATE_CONNECTED;
    APPL_TRACE_DEBUG("%s: Return device status %d", __FUNCTION__, p_dev->dev_status);
}
