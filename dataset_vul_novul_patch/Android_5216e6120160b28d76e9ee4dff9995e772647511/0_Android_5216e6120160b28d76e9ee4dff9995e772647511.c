void mca_ccb_hdl_req(tMCA_CCB* p_ccb, tMCA_CCB_EVT* p_data) {
  BT_HDR* p_pkt = &p_data->hdr;
 uint8_t *p, *p_start;
  tMCA_DCB* p_dcb;
  tMCA_CTRL evt_data;
  tMCA_CCB_MSG* p_rx_msg = NULL;
 uint8_t reject_code = MCA_RSP_NO_RESOURCE;
 bool send_rsp = false;
 bool check_req = false;
 uint8_t reject_opcode;

  MCA_TRACE_DEBUG("mca_ccb_hdl_req status:%d", p_ccb->status);

   p_rx_msg = (tMCA_CCB_MSG*)p_pkt;
   p = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
   evt_data.hdr.op_code = *p++;
   reject_opcode = evt_data.hdr.op_code + 1;
 
  if (p_pkt->len >= 3) {
    BE_STREAM_TO_UINT16(evt_data.hdr.mdl_id, p);
  } else {
    android_errorWriteLog(0x534e4554, "110791536");
    evt_data.hdr.mdl_id = 0;
  }

   MCA_TRACE_DEBUG("received mdl id: %d ", evt_data.hdr.mdl_id);
   if (p_ccb->status == MCA_CCB_STAT_PENDING) {
     MCA_TRACE_DEBUG("received req inpending state");
 /* allow abort in pending state */
 if ((p_ccb->status == MCA_CCB_STAT_PENDING) &&
 (evt_data.hdr.op_code == MCA_OP_MDL_ABORT_REQ)) {
      reject_code = MCA_RSP_SUCCESS;
      send_rsp = true;
 /* clear the pending status */
      p_ccb->status = MCA_CCB_STAT_NORM;
 if (p_ccb->p_tx_req &&
 ((p_dcb = mca_dcb_by_hdl(p_ccb->p_tx_req->dcb_idx)) != NULL)) {
        mca_dcb_dealloc(p_dcb, NULL);
        osi_free_and_reset((void**)&p_ccb->p_tx_req);
 }
 } else
      reject_code = MCA_RSP_BAD_OP;
 } else if (p_ccb->p_rx_msg) {
    MCA_TRACE_DEBUG("still handling prev req");
 /* still holding previous message, reject this new one ?? */

 } else if (p_ccb->p_tx_req) {
    MCA_TRACE_DEBUG("still waiting for a response ctrl_vpsm:0x%x",
                    p_ccb->ctrl_vpsm);
 /* sent a request; waiting for response */
 if (p_ccb->ctrl_vpsm == 0) {
      MCA_TRACE_DEBUG("local is ACP. accept the cmd from INT");
 /* local is acceptor, need to handle the request */
      check_req = true;
      reject_code = MCA_RSP_SUCCESS;
 /* drop the previous request */
 if ((p_ccb->p_tx_req->op_code == MCA_OP_MDL_CREATE_REQ) &&
 ((p_dcb = mca_dcb_by_hdl(p_ccb->p_tx_req->dcb_idx)) != NULL)) {
        mca_dcb_dealloc(p_dcb, NULL);
 }
      osi_free_and_reset((void**)&p_ccb->p_tx_req);
      mca_stop_timer(p_ccb);
 } else {
 /*  local is initiator, ignore the req */
      osi_free(p_pkt);
 return;
 }
 } else if (p_pkt->layer_specific != MCA_RSP_SUCCESS) {
    reject_code = (uint8_t)p_pkt->layer_specific;
 if (((evt_data.hdr.op_code >= MCA_NUM_STANDARD_OPCODE) &&
 (evt_data.hdr.op_code < MCA_FIRST_SYNC_OP)) ||
 (evt_data.hdr.op_code > MCA_LAST_SYNC_OP)) {
 /* invalid op code */
      reject_opcode = MCA_OP_ERROR_RSP;
      evt_data.hdr.mdl_id = 0;
 }
 } else {
    check_req = true;
    reject_code = MCA_RSP_SUCCESS;
 }

 if (check_req) {
 if (reject_code == MCA_RSP_SUCCESS) {
      reject_code = MCA_RSP_BAD_MDL;
 if (MCA_IS_VALID_MDL_ID(evt_data.hdr.mdl_id) ||
 ((evt_data.hdr.mdl_id == MCA_ALL_MDL_ID) &&
 (evt_data.hdr.op_code == MCA_OP_MDL_DELETE_REQ))) {
        reject_code = MCA_RSP_SUCCESS;
 /* mdl_id is valid according to the spec */
 switch (evt_data.hdr.op_code) {
 case MCA_OP_MDL_CREATE_REQ:
            evt_data.create_ind.dep_id = *p++;
            evt_data.create_ind.cfg = *p++;
            p_rx_msg->mdep_id = evt_data.create_ind.dep_id;
 if (!mca_is_valid_dep_id(p_ccb->p_rcb, p_rx_msg->mdep_id)) {
              MCA_TRACE_ERROR("%s: Invalid local MDEP ID %d", __func__,
                              p_rx_msg->mdep_id);
              reject_code = MCA_RSP_BAD_MDEP;
 } else if (mca_ccb_uses_mdl_id(p_ccb, evt_data.hdr.mdl_id)) {
              MCA_TRACE_DEBUG("the mdl_id is currently used in the CL(create)");
              mca_dcb_close_by_mdl_id(p_ccb, evt_data.hdr.mdl_id);
 } else {
 /* check if this dep still have MDL available */
 if (mca_dep_free_mdl(p_ccb, evt_data.create_ind.dep_id) == 0) {
                MCA_TRACE_ERROR("%s: MAX_MDL is used by MDEP %d", __func__,
                                evt_data.create_ind.dep_id);
                reject_code = MCA_RSP_MDEP_BUSY;
 }
 }
 break;

 case MCA_OP_MDL_RECONNECT_REQ:
 if (mca_ccb_uses_mdl_id(p_ccb, evt_data.hdr.mdl_id)) {
              MCA_TRACE_ERROR("%s: MDL_ID %d busy, in CL(reconn)", __func__,
                              evt_data.hdr.mdl_id);
              reject_code = MCA_RSP_MDL_BUSY;
 }
 break;

 case MCA_OP_MDL_ABORT_REQ:
            reject_code = MCA_RSP_BAD_OP;
 break;

 case MCA_OP_MDL_DELETE_REQ:
 /* delete the associated mdl */
            mca_dcb_close_by_mdl_id(p_ccb, evt_data.hdr.mdl_id);
            send_rsp = true;
 break;
 }
 }
 }
 }

 if (((reject_code != MCA_RSP_SUCCESS) &&
 (evt_data.hdr.op_code != MCA_OP_SYNC_INFO_IND)) ||
      send_rsp) {
    BT_HDR* p_buf = (BT_HDR*)osi_malloc(MCA_CTRL_MTU + sizeof(BT_HDR));
    p_buf->offset = L2CAP_MIN_OFFSET;
    p = p_start = (uint8_t*)(p_buf + 1) + L2CAP_MIN_OFFSET;
 *p++ = reject_opcode;
 *p++ = reject_code;
 bool valid_response = true;
 switch (reject_opcode) {
 case MCA_OP_ERROR_RSP:
 case MCA_OP_MDL_CREATE_RSP:
 case MCA_OP_MDL_RECONNECT_RSP:
 case MCA_OP_MDL_ABORT_RSP:
 case MCA_OP_MDL_DELETE_RSP:
        UINT16_TO_BE_STREAM(p, evt_data.hdr.mdl_id);
 break;
 case MCA_OP_SYNC_CAP_RSP:
        memset(p, 0, 7);
        p += 7;
 break;
 case MCA_OP_SYNC_SET_RSP:
        memset(p, 0, 14);
        p += 14;
 break;
 default:
        MCA_TRACE_ERROR("%s: reject_opcode 0x%02x not recognized", __func__,
                        reject_opcode);
        valid_response = false;
 break;
 }
 if (valid_response) {
      p_buf->len = p - p_start;
      MCA_TRACE_ERROR("%s: reject_opcode=0x%02x, reject_code=0x%02x, length=%d",
                      __func__, reject_opcode, reject_code, p_buf->len);
      L2CA_DataWrite(p_ccb->lcid, p_buf);
 } else {
      osi_free(p_buf);
 }
 }

 if (reject_code == MCA_RSP_SUCCESS) {
 /* use the received GKI buffer to store information to double check response
     * API */
    p_rx_msg->op_code = evt_data.hdr.op_code;
    p_rx_msg->mdl_id = evt_data.hdr.mdl_id;
    p_ccb->p_rx_msg = p_rx_msg;
 if (send_rsp) {
      osi_free(p_pkt);
      p_ccb->p_rx_msg = NULL;
 }
    mca_ccb_report_event(p_ccb, evt_data.hdr.op_code, &evt_data);
 } else
    osi_free(p_pkt);
}
