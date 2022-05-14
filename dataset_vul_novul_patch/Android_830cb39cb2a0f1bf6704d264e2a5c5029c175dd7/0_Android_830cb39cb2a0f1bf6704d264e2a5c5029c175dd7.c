static void avrc_msg_cback(uint8_t handle, uint8_t label, uint8_t cr,
                           BT_HDR* p_pkt) {
 uint8_t opcode;
  tAVRC_MSG msg;
 uint8_t* p_data;
 uint8_t* p_begin;
 bool drop = false;
 bool do_free = true;
  BT_HDR* p_rsp = NULL;
 uint8_t* p_rsp_data;
 int xx;
 bool reject = false;
 const char* p_drop_msg = "dropped";
  tAVRC_MSG_VENDOR* p_msg = &msg.vendor;

 if (cr == AVCT_CMD && (p_pkt->layer_specific & AVCT_DATA_CTRL &&
                         AVRC_PACKET_LEN < sizeof(p_pkt->len))) {
 /* Ignore the invalid AV/C command frame */
    p_drop_msg = "dropped - too long AV/C cmd frame size";
    osi_free(p_pkt);
 return;
 }

 if (cr == AVCT_REJ) {
 /* The peer thinks that this PID is no longer open - remove this handle */
 /*  */
    osi_free(p_pkt);
    AVCT_RemoveConn(handle);
 return;
 } else if (cr == AVCT_RSP) {
 /* Received response. Stop command timeout timer */
    AVRC_TRACE_DEBUG("AVRC: stopping timer (handle=0x%02x)", handle);
    alarm_cancel(avrc_cb.ccb_int[handle].tle);
 }

  p_data = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
  memset(&msg, 0, sizeof(tAVRC_MSG));

 if (p_pkt->layer_specific == AVCT_DATA_BROWSE) {
    opcode = AVRC_OP_BROWSE;
    msg.browse.hdr.ctype = cr;
    msg.browse.p_browse_data = p_data;

     msg.browse.browse_len = p_pkt->len;
     msg.browse.p_browse_pkt = p_pkt;
   } else {
    if (p_pkt->len < AVRC_AVC_HDR_SIZE) {
      android_errorWriteLog(0x534e4554, "111803925");
      AVRC_TRACE_WARNING("%s: message length %d too short: must be at least %d",
                         __func__, p_pkt->len, AVRC_AVC_HDR_SIZE);
      osi_free(p_pkt);
      return;
    }
     msg.hdr.ctype = p_data[0] & AVRC_CTYPE_MASK;
     AVRC_TRACE_DEBUG("%s handle:%d, ctype:%d, offset:%d, len: %d", __func__,
                      handle, msg.hdr.ctype, p_pkt->offset, p_pkt->len);
    msg.hdr.subunit_type =
 (p_data[1] & AVRC_SUBTYPE_MASK) >> AVRC_SUBTYPE_SHIFT;
    msg.hdr.subunit_id = p_data[1] & AVRC_SUBID_MASK;
    opcode = p_data[2];
 }

 if (((avrc_cb.ccb[handle].control & AVRC_CT_TARGET) && (cr == AVCT_CMD)) ||
 ((avrc_cb.ccb[handle].control & AVRC_CT_CONTROL) && (cr == AVCT_RSP))) {
 switch (opcode) {
 case AVRC_OP_UNIT_INFO:
 if (cr == AVCT_CMD) {
 /* send the response to the peer */
          p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_UNIT_INFO_RSP_LEN);
          p_rsp_data = avrc_get_data_ptr(p_rsp);
 *p_rsp_data = AVRC_RSP_IMPL_STBL;
 /* check & set the offset. set response code, set subunit_type &
             subunit_id,
             set AVRC_OP_UNIT_INFO */
 /* 3 bytes: ctype, subunit*, opcode */
          p_rsp_data += AVRC_AVC_HDR_SIZE;
 *p_rsp_data++ = 7;
 /* Panel subunit & id=0 */
 *p_rsp_data++ = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
          AVRC_CO_ID_TO_BE_STREAM(p_rsp_data, avrc_cb.ccb[handle].company_id);
          p_rsp->len =
 (uint16_t)(p_rsp_data - (uint8_t*)(p_rsp + 1) - p_rsp->offset);
          cr = AVCT_RSP;

           p_drop_msg = "auto respond";
         } else {
           /* parse response */
          if (p_pkt->len < AVRC_OP_UNIT_INFO_RSP_LEN) {
            AVRC_TRACE_WARNING(
                "%s: message length %d too short: must be at least %d",
                __func__, p_pkt->len, AVRC_OP_UNIT_INFO_RSP_LEN);
            android_errorWriteLog(0x534e4554, "79883824");
            drop = true;
            p_drop_msg = "UNIT_INFO_RSP too short";
            break;
          }
           p_data += 4; /* 3 bytes: ctype, subunit*, opcode + octet 3 (is 7)*/
           msg.unit.unit_type =
               (*p_data & AVRC_SUBTYPE_MASK) >> AVRC_SUBTYPE_SHIFT;
          msg.unit.unit = *p_data & AVRC_SUBID_MASK;
          p_data++;
          AVRC_BE_STREAM_TO_CO_ID(msg.unit.company_id, p_data);
 }
 break;

 case AVRC_OP_SUB_INFO:
 if (cr == AVCT_CMD) {
 /* send the response to the peer */
          p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_SUB_UNIT_INFO_RSP_LEN);
          p_rsp_data = avrc_get_data_ptr(p_rsp);
 *p_rsp_data = AVRC_RSP_IMPL_STBL;
 /* check & set the offset. set response code, set (subunit_type &
             subunit_id),
             set AVRC_OP_SUB_INFO, set (page & extention code) */
          p_rsp_data += 4;
 /* Panel subunit & id=0 */
 *p_rsp_data++ = (AVRC_SUB_PANEL << AVRC_SUBTYPE_SHIFT);
          memset(p_rsp_data, AVRC_CMD_OPRND_PAD, AVRC_SUBRSP_OPRND_BYTES);
          p_rsp_data += AVRC_SUBRSP_OPRND_BYTES;
          p_rsp->len =
 (uint16_t)(p_rsp_data - (uint8_t*)(p_rsp + 1) - p_rsp->offset);
          cr = AVCT_RSP;

           p_drop_msg = "auto responded";
         } else {
           /* parse response */
          if (p_pkt->len < AVRC_OP_SUB_UNIT_INFO_RSP_LEN) {
            AVRC_TRACE_WARNING(
                "%s: message length %d too short: must be at least %d",
                __func__, p_pkt->len, AVRC_OP_SUB_UNIT_INFO_RSP_LEN);
            android_errorWriteLog(0x534e4554, "79883824");
            drop = true;
            p_drop_msg = "SUB_UNIT_INFO_RSP too short";
            break;
          }
           p_data += AVRC_AVC_HDR_SIZE; /* 3 bytes: ctype, subunit*, opcode */
           msg.sub.page =
               (*p_data++ >> AVRC_SUB_PAGE_SHIFT) & AVRC_SUB_PAGE_MASK;
          xx = 0;
 while (*p_data != AVRC_CMD_OPRND_PAD && xx < AVRC_SUB_TYPE_LEN) {
            msg.sub.subunit_type[xx] = *p_data++ >> AVRC_SUBTYPE_SHIFT;
 if (msg.sub.subunit_type[xx] == AVRC_SUB_PANEL)
              msg.sub.panel = true;
            xx++;
 }
 }
 break;

 case AVRC_OP_VENDOR: {
        p_data = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
        p_begin = p_data;
 if (p_pkt->len <
            AVRC_VENDOR_HDR_SIZE) /* 6 = ctype, subunit*, opcode & CO_ID */
 {
 if (cr == AVCT_CMD)
            reject = true;
 else
            drop = true;
 break;
 }
        p_data += AVRC_AVC_HDR_SIZE; /* skip the first 3 bytes: ctype, subunit*,
                                        opcode */
        AVRC_BE_STREAM_TO_CO_ID(p_msg->company_id, p_data);
        p_msg->p_vendor_data = p_data;
        p_msg->vendor_len = p_pkt->len - (p_data - p_begin);

 uint8_t drop_code = 0;
 if (p_msg->company_id == AVRC_CO_METADATA) {
 /* Validate length for metadata message */
 if (p_pkt->len < (AVRC_VENDOR_HDR_SIZE + AVRC_MIN_META_HDR_SIZE)) {
 if (cr == AVCT_CMD)
              reject = true;
 else
              drop = true;
 break;
 }

 /* Check+handle fragmented messages */
          drop_code = avrc_proc_far_msg(handle, label, cr, &p_pkt, p_msg);
 if (drop_code > 0) drop = true;
 }
 if (drop_code > 0) {
 if (drop_code != 4) do_free = false;
 switch (drop_code) {
 case 1:
              p_drop_msg = "sent_frag";
 break;
 case 2:
              p_drop_msg = "req_cont";
 break;
 case 3:
              p_drop_msg = "sent_frag3";
 break;
 case 4:
              p_drop_msg = "sent_frag_free";
 break;
 default:
              p_drop_msg = "sent_fragd";
 }
 }
 /* If vendor response received, and did not ask for continuation */
 /* then check queue for addition commands to send */
 if ((cr == AVCT_RSP) && (drop_code != 2)) {
          avrc_send_next_vendor_cmd(handle);
 }
 } break;

 case AVRC_OP_PASS_THRU:
 if (p_pkt->len < 5) /* 3 bytes: ctype, subunit*, opcode & op_id & len */
 {
 if (cr == AVCT_CMD)
            reject = true;
 else
            drop = true;
 break;
 }
        p_data += AVRC_AVC_HDR_SIZE; /* skip the first 3 bytes: ctype, subunit*,
                                        opcode */
        msg.pass.op_id = (AVRC_PASS_OP_ID_MASK & *p_data);
 if (AVRC_PASS_STATE_MASK & *p_data)
          msg.pass.state = true;
 else
          msg.pass.state = false;
        p_data++;
        msg.pass.pass_len = *p_data++;
 if (msg.pass.pass_len != p_pkt->len - 5)
          msg.pass.pass_len = p_pkt->len - 5;
 if (msg.pass.pass_len)
          msg.pass.p_pass_data = p_data;
 else
          msg.pass.p_pass_data = NULL;
 break;

 case AVRC_OP_BROWSE:
 /* If browse response received, then check queue for addition commands
         * to send */
 if (cr == AVCT_RSP) {
          avrc_send_next_vendor_cmd(handle);
 }
 break;

 default:
 if ((avrc_cb.ccb[handle].control & AVRC_CT_TARGET) &&
 (cr == AVCT_CMD)) {
 /* reject unsupported opcode */
          reject = true;
 }
        drop = true;
 break;
 }
 } else /* drop the event */
 {
 if (opcode != AVRC_OP_BROWSE) drop = true;
 }

 if (reject) {
 /* reject unsupported opcode */
    p_rsp = avrc_copy_packet(p_pkt, AVRC_OP_REJ_MSG_LEN);
    p_rsp_data = avrc_get_data_ptr(p_rsp);
 *p_rsp_data = AVRC_RSP_REJ;
    p_drop_msg = "rejected";
    cr = AVCT_RSP;
    drop = true;
 }

 if (p_rsp) {
 /* set to send response right away */
    AVCT_MsgReq(handle, label, cr, p_rsp);
    drop = true;
 }

 if (!drop) {
    msg.hdr.opcode = opcode;
    avrc_cb.ccb[handle].msg_cback.Run(handle, label, opcode, &msg);
 } else {
    AVRC_TRACE_WARNING("%s %s msg handle:%d, control:%d, cr:%d, opcode:x%x",
                       __func__, p_drop_msg, handle,
                       avrc_cb.ccb[handle].control, cr, opcode);
 }

 if (opcode == AVRC_OP_BROWSE && msg.browse.p_browse_pkt == NULL) {
    do_free = false;
 }

 if (do_free) osi_free(p_pkt);
}
