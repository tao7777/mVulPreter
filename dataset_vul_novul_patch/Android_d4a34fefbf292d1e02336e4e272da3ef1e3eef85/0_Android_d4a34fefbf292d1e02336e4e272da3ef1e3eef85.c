uint8_t rfc_parse_data(tRFC_MCB* p_mcb, MX_FRAME* p_frame, BT_HDR* p_buf) {
 uint8_t ead, eal, fcs;
 uint8_t* p_data = (uint8_t*)(p_buf + 1) + p_buf->offset;
 uint8_t* p_start = p_data;
 uint16_t len;

 if (p_buf->len < RFCOMM_CTRL_FRAME_LEN) {
    RFCOMM_TRACE_ERROR("Bad Length1: %d", p_buf->len);
 return (RFC_EVENT_BAD_FRAME);
 }

  RFCOMM_PARSE_CTRL_FIELD(ead, p_frame->cr, p_frame->dlci, p_data);
 if (!ead) {
    RFCOMM_TRACE_ERROR("Bad Address(EA must be 1)");
 return (RFC_EVENT_BAD_FRAME);
 }
  RFCOMM_PARSE_TYPE_FIELD(p_frame->type, p_frame->pf, p_data);

 
   eal = *(p_data)&RFCOMM_EA;
   len = *(p_data)++ >> RFCOMM_SHIFT_LENGTH1;
  if (eal == 0 && p_buf->len > RFCOMM_CTRL_FRAME_LEN) {
     len += (*(p_data)++ << RFCOMM_SHIFT_LENGTH2);
   } else if (eal == 0) {
     RFCOMM_TRACE_ERROR("Bad Length when EAL = 0: %d", p_buf->len);
    android_errorWriteLog(0x534e4554, "78288018");
 return RFC_EVENT_BAD_FRAME;
 }

  p_buf->len -= (3 + !ead + !eal + 1); /* Additional 1 for FCS */
  p_buf->offset += (3 + !ead + !eal);

 /* handle credit if credit based flow control */
 if ((p_mcb->flow == PORT_FC_CREDIT) && (p_frame->type == RFCOMM_UIH) &&
 (p_frame->dlci != RFCOMM_MX_DLCI) && (p_frame->pf == 1)) {
    p_frame->credit = *p_data++;
    p_buf->len--;
    p_buf->offset++;
 } else
    p_frame->credit = 0;

 if (p_buf->len != len) {
    RFCOMM_TRACE_ERROR("Bad Length2 %d %d", p_buf->len, len);
 return (RFC_EVENT_BAD_FRAME);
 }

  fcs = *(p_data + len);

 /* All control frames that we are sending are sent with P=1, expect */
 /* reply with F=1 */
 /* According to TS 07.10 spec ivalid frames are discarded without */
 /* notification to the sender */
 switch (p_frame->type) {
 case RFCOMM_SABME:
 if (RFCOMM_FRAME_IS_RSP(p_mcb->is_initiator, p_frame->cr) ||
 !p_frame->pf || len || !RFCOMM_VALID_DLCI(p_frame->dlci) ||
 !rfc_check_fcs(RFCOMM_CTRL_FRAME_LEN, p_start, fcs)) {
        RFCOMM_TRACE_ERROR("Bad SABME");
 return (RFC_EVENT_BAD_FRAME);
 } else
 return (RFC_EVENT_SABME);

 case RFCOMM_UA:
 if (RFCOMM_FRAME_IS_CMD(p_mcb->is_initiator, p_frame->cr) ||
 !p_frame->pf || len || !RFCOMM_VALID_DLCI(p_frame->dlci) ||
 !rfc_check_fcs(RFCOMM_CTRL_FRAME_LEN, p_start, fcs)) {
        RFCOMM_TRACE_ERROR("Bad UA");
 return (RFC_EVENT_BAD_FRAME);
 } else
 return (RFC_EVENT_UA);

 case RFCOMM_DM:
 if (RFCOMM_FRAME_IS_CMD(p_mcb->is_initiator, p_frame->cr) || len ||
 !RFCOMM_VALID_DLCI(p_frame->dlci) ||
 !rfc_check_fcs(RFCOMM_CTRL_FRAME_LEN, p_start, fcs)) {
        RFCOMM_TRACE_ERROR("Bad DM");
 return (RFC_EVENT_BAD_FRAME);
 } else
 return (RFC_EVENT_DM);

 case RFCOMM_DISC:
 if (RFCOMM_FRAME_IS_RSP(p_mcb->is_initiator, p_frame->cr) ||
 !p_frame->pf || len || !RFCOMM_VALID_DLCI(p_frame->dlci) ||
 !rfc_check_fcs(RFCOMM_CTRL_FRAME_LEN, p_start, fcs)) {
        RFCOMM_TRACE_ERROR("Bad DISC");
 return (RFC_EVENT_BAD_FRAME);
 } else
 return (RFC_EVENT_DISC);

 case RFCOMM_UIH:
 if (!RFCOMM_VALID_DLCI(p_frame->dlci)) {
        RFCOMM_TRACE_ERROR("Bad UIH - invalid DLCI");
 return (RFC_EVENT_BAD_FRAME);
 } else if (!rfc_check_fcs(2, p_start, fcs)) {
        RFCOMM_TRACE_ERROR("Bad UIH - FCS");
 return (RFC_EVENT_BAD_FRAME);
 } else if (RFCOMM_FRAME_IS_RSP(p_mcb->is_initiator, p_frame->cr)) {
 /* we assume that this is ok to allow bad implementations to work */
        RFCOMM_TRACE_ERROR("Bad UIH - response");
 return (RFC_EVENT_UIH);
 } else
 return (RFC_EVENT_UIH);
 }

 return (RFC_EVENT_BAD_FRAME);
}
