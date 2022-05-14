static int uhid_event(btif_hh_device_t *p_dev)
{
 struct uhid_event ev;
 ssize_t ret;
    memset(&ev, 0, sizeof(ev));
 if(!p_dev)
 {

         APPL_TRACE_ERROR("%s: Device not found",__FUNCTION__)
         return -1;
     }
    ret = read(p_dev->fd, &ev, sizeof(ev));
     if (ret == 0) {
         APPL_TRACE_ERROR("%s: Read HUP on uhid-cdev %s", __FUNCTION__,
                                                  strerror(errno));
 return -EFAULT;
 } else if (ret < 0) {
        APPL_TRACE_ERROR("%s: Cannot read uhid-cdev: %s", __FUNCTION__,
                                                strerror(errno));
 return -errno;
 } else if ((ev.type == UHID_OUTPUT) || (ev.type==UHID_OUTPUT_EV)) {
 if (ret < (ssize_t)sizeof(ev)) {
            APPL_TRACE_ERROR("%s: Invalid size read from uhid-dev: %ld != %lu",
                         __FUNCTION__, ret, sizeof(ev.type));
 return -EFAULT;
 }
 }

 switch (ev.type) {
 case UHID_START:
        APPL_TRACE_DEBUG("UHID_START from uhid-dev\n");
        p_dev->ready_for_data = TRUE;
 break;
 case UHID_STOP:
        APPL_TRACE_DEBUG("UHID_STOP from uhid-dev\n");
        p_dev->ready_for_data = FALSE;
 break;
 case UHID_OPEN:
        APPL_TRACE_DEBUG("UHID_OPEN from uhid-dev\n");
 break;
 case UHID_CLOSE:
        APPL_TRACE_DEBUG("UHID_CLOSE from uhid-dev\n");
        p_dev->ready_for_data = FALSE;
 break;
 case UHID_OUTPUT:
 if (ret < (ssize_t)(sizeof(ev.type) + sizeof(ev.u.output))) {
            APPL_TRACE_ERROR("%s: Invalid size read from uhid-dev: %zd < %zu",
                             __FUNCTION__, ret,
 sizeof(ev.type) + sizeof(ev.u.output));
 return -EFAULT;
 }

        APPL_TRACE_DEBUG("UHID_OUTPUT: Report type = %d, report_size = %d"
 ,ev.u.output.rtype, ev.u.output.size);
 if(ev.u.output.rtype == UHID_FEATURE_REPORT)
            btif_hh_setreport(p_dev, BTHH_FEATURE_REPORT,
                              ev.u.output.size, ev.u.output.data);
 else if(ev.u.output.rtype == UHID_OUTPUT_REPORT)
            btif_hh_setreport(p_dev, BTHH_OUTPUT_REPORT,
                              ev.u.output.size, ev.u.output.data);
 else
            btif_hh_setreport(p_dev, BTHH_INPUT_REPORT,
                              ev.u.output.size, ev.u.output.data);
 break;
 case UHID_OUTPUT_EV:
        APPL_TRACE_DEBUG("UHID_OUTPUT_EV from uhid-dev\n");
 break;
 case UHID_FEATURE:
        APPL_TRACE_DEBUG("UHID_FEATURE from uhid-dev\n");
 break;
 case UHID_FEATURE_ANSWER:
        APPL_TRACE_DEBUG("UHID_FEATURE_ANSWER from uhid-dev\n");
 break;

 default:
        APPL_TRACE_DEBUG("Invalid event from uhid-dev: %u\n", ev.type);
 }

 return 0;
}
