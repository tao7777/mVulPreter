void rfc_process_mx_message(tRFC_MCB* p_mcb, BT_HDR* p_buf) {
 uint8_t* p_data = (uint8_t*)(p_buf + 1) + p_buf->offset;
  MX_FRAME* p_rx_frame = &rfc_cb.rfc.rx_frame;
 uint16_t length = p_buf->len;

   uint8_t ea, cr, mx_len;
   bool is_command;
 
  if (length < 2) {
    RFCOMM_TRACE_ERROR(
        "%s: Illegal MX Frame len when reading EA, C/R. len:%d < 2", __func__,
        length);
    android_errorWriteLog(0x534e4554, "111937065");
    osi_free(p_buf);
    return;
  }
   p_rx_frame->ea = *p_data & RFCOMM_EA;
   p_rx_frame->cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;
   p_rx_frame->type = *p_data++ & ~(RFCOMM_CR_MASK | RFCOMM_EA_MASK);

 if (!p_rx_frame->ea || !length) {
    LOG(ERROR) << __func__
 << ": Invalid MX frame ea=" << std::to_string(p_rx_frame->ea)
 << ", len=" << length << ", bd_addr=" << p_mcb->bd_addr;
    osi_free(p_buf);
 return;
 }

  length--;

  is_command = p_rx_frame->cr;

  ea = *p_data & RFCOMM_EA;

  mx_len = *p_data++ >> RFCOMM_SHIFT_LENGTH1;

   length--;
 
   if (!ea) {
    if (length < 1) {
      RFCOMM_TRACE_ERROR("%s: Illegal MX Frame when EA = 0. len:%d < 1",
                         __func__, length);
      android_errorWriteLog(0x534e4554, "111937065");
      osi_free(p_buf);
      return;
    }
     mx_len += *p_data++ << RFCOMM_SHIFT_LENGTH2;
     length--;
   }

 if (mx_len != length) {
    LOG(ERROR) << __func__ << ": Bad MX frame, p_mcb=" << p_mcb
 << ", bd_addr=" << p_mcb->bd_addr;
    osi_free(p_buf);
 return;
 }

  RFCOMM_TRACE_DEBUG("%s: type=%d, p_mcb=%p", __func__, p_rx_frame->type,
                     p_mcb);
 switch (p_rx_frame->type) {
 case RFCOMM_MX_PN:
 if (length != RFCOMM_MX_PN_LEN) {
        LOG(ERROR) << __func__ << ": Invalid PN length, p_mcb=" << p_mcb
 << ", bd_addr=" << p_mcb->bd_addr;
 break;
 }

      p_rx_frame->dlci = *p_data++ & RFCOMM_PN_DLCI_MASK;
      p_rx_frame->u.pn.frame_type = *p_data & RFCOMM_PN_FRAME_TYPE_MASK;
      p_rx_frame->u.pn.conv_layer = *p_data++ & RFCOMM_PN_CONV_LAYER_MASK;
      p_rx_frame->u.pn.priority = *p_data++ & RFCOMM_PN_PRIORITY_MASK;
      p_rx_frame->u.pn.t1 = *p_data++;
      p_rx_frame->u.pn.mtu = *p_data + (*(p_data + 1) << 8);
      p_data += 2;
      p_rx_frame->u.pn.n2 = *p_data++;
      p_rx_frame->u.pn.k = *p_data++ & RFCOMM_PN_K_MASK;

 if (!p_rx_frame->dlci || !RFCOMM_VALID_DLCI(p_rx_frame->dlci) ||
 (p_rx_frame->u.pn.mtu < RFCOMM_MIN_MTU) ||
 (p_rx_frame->u.pn.mtu > RFCOMM_MAX_MTU)) {
        LOG(ERROR) << __func__ << ": Bad PN frame, p_mcb=" << p_mcb
 << ", bd_addr=" << p_mcb->bd_addr;
 break;
 }

      osi_free(p_buf);

      rfc_process_pn(p_mcb, is_command, p_rx_frame);
 return;

 case RFCOMM_MX_TEST:
 if (!length) break;

      p_rx_frame->u.test.p_data = p_data;
      p_rx_frame->u.test.data_len = length;

      p_buf->offset += 2;
      p_buf->len -= 2;

 if (is_command)
        rfc_send_test(p_mcb, false, p_buf);
 else
        rfc_process_test_rsp(p_mcb, p_buf);
 return;

 case RFCOMM_MX_FCON:
 if (length != RFCOMM_MX_FCON_LEN) break;

      osi_free(p_buf);

      rfc_process_fcon(p_mcb, is_command);
 return;

 case RFCOMM_MX_FCOFF:
 if (length != RFCOMM_MX_FCOFF_LEN) break;

      osi_free(p_buf);

      rfc_process_fcoff(p_mcb, is_command);

       return;
 
     case RFCOMM_MX_MSC:
      if (length != RFCOMM_MX_MSC_LEN_WITH_BREAK &&
          length != RFCOMM_MX_MSC_LEN_NO_BREAK) {
        RFCOMM_TRACE_ERROR("%s: Illegal MX MSC Frame len:%d", __func__, length);
        android_errorWriteLog(0x534e4554, "111937065");
        osi_free(p_buf);
        return;
      }
       ea = *p_data & RFCOMM_EA;
       cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;
       p_rx_frame->dlci = *p_data++ >> RFCOMM_SHIFT_DLCI;

 if (!ea || !cr || !p_rx_frame->dlci ||
 !RFCOMM_VALID_DLCI(p_rx_frame->dlci)) {
        RFCOMM_TRACE_ERROR("Bad MSC frame");
 break;
 }

      p_rx_frame->u.msc.signals = *p_data++;

 if (mx_len == RFCOMM_MX_MSC_LEN_WITH_BREAK) {
        p_rx_frame->u.msc.break_present =
 *p_data & RFCOMM_MSC_BREAK_PRESENT_MASK;
        p_rx_frame->u.msc.break_duration =
 (*p_data & RFCOMM_MSC_BREAK_MASK) >> RFCOMM_MSC_SHIFT_BREAK;
 } else {
        p_rx_frame->u.msc.break_present = false;
        p_rx_frame->u.msc.break_duration = 0;
 }
      osi_free(p_buf);

      rfc_process_msc(p_mcb, is_command, p_rx_frame);
 return;

 case RFCOMM_MX_NSC:
 if ((length != RFCOMM_MX_NSC_LEN) || !is_command) break;

      p_rx_frame->u.nsc.ea = *p_data & RFCOMM_EA;
      p_rx_frame->u.nsc.cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;
      p_rx_frame->u.nsc.type = *p_data++ >> RFCOMM_SHIFT_DLCI;

      osi_free(p_buf);

      rfc_process_nsc(p_mcb, p_rx_frame);
 return;

 case RFCOMM_MX_RPN:
 if ((length != RFCOMM_MX_RPN_REQ_LEN) && (length != RFCOMM_MX_RPN_LEN))
 break;

      ea = *p_data & RFCOMM_EA;
      cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;
      p_rx_frame->dlci = *p_data++ >> RFCOMM_SHIFT_DLCI;

 if (!ea || !cr || !p_rx_frame->dlci ||
 !RFCOMM_VALID_DLCI(p_rx_frame->dlci)) {
        RFCOMM_TRACE_ERROR("Bad RPN frame");
 break;
 }

      p_rx_frame->u.rpn.is_request = (length == RFCOMM_MX_RPN_REQ_LEN);

 if (!p_rx_frame->u.rpn.is_request) {
        p_rx_frame->u.rpn.baud_rate = *p_data++;
        p_rx_frame->u.rpn.byte_size =
 (*p_data >> RFCOMM_RPN_BITS_SHIFT) & RFCOMM_RPN_BITS_MASK;
        p_rx_frame->u.rpn.stop_bits =
 (*p_data >> RFCOMM_RPN_STOP_BITS_SHIFT) & RFCOMM_RPN_STOP_BITS_MASK;
        p_rx_frame->u.rpn.parity =
 (*p_data >> RFCOMM_RPN_PARITY_SHIFT) & RFCOMM_RPN_PARITY_MASK;
        p_rx_frame->u.rpn.parity_type =
 (*p_data++ >> RFCOMM_RPN_PARITY_TYPE_SHIFT) &
            RFCOMM_RPN_PARITY_TYPE_MASK;

        p_rx_frame->u.rpn.fc_type = *p_data++ & RFCOMM_FC_MASK;
        p_rx_frame->u.rpn.xon_char = *p_data++;
        p_rx_frame->u.rpn.xoff_char = *p_data++;
        p_rx_frame->u.rpn.param_mask =
 (*p_data + (*(p_data + 1) << 8)) & RFCOMM_RPN_PM_MASK;
 }
      osi_free(p_buf);

      rfc_process_rpn(p_mcb, is_command, p_rx_frame->u.rpn.is_request,
                      p_rx_frame);
 return;

 case RFCOMM_MX_RLS:
 if (length != RFCOMM_MX_RLS_LEN) break;

      ea = *p_data & RFCOMM_EA;
      cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;

      p_rx_frame->dlci = *p_data++ >> RFCOMM_SHIFT_DLCI;
      p_rx_frame->u.rls.line_status = (*p_data & ~0x01);

 if (!ea || !cr || !p_rx_frame->dlci ||
 !RFCOMM_VALID_DLCI(p_rx_frame->dlci)) {
        RFCOMM_TRACE_ERROR("Bad RPN frame");
 break;
 }

      osi_free(p_buf);

      rfc_process_rls(p_mcb, is_command, p_rx_frame);
 return;
 }

  osi_free(p_buf);

 if (is_command) rfc_send_nsc(p_mcb);
}
