static void btif_av_event_free_data(btif_sm_event_t event, void* p_data) {
 switch (event) {
 case BTA_AV_META_MSG_EVT: {
      tBTA_AV* av = (tBTA_AV*)p_data;
      osi_free_and_reset((void**)&av->meta_msg.p_data);

 if (av->meta_msg.p_msg) {

         if (av->meta_msg.p_msg->hdr.opcode == AVRC_OP_VENDOR) {
           osi_free(av->meta_msg.p_msg->vendor.p_vendor_data);
         }
        if (av->meta_msg.p_msg->hdr.opcode == AVRC_OP_BROWSE) {
          osi_free(av->meta_msg.p_msg->browse.p_browse_data);
        }
         osi_free_and_reset((void**)&av->meta_msg.p_msg);
       }
     } break;

 default:
 break;
 }
}
