void btif_av_event_deep_copy(uint16_t event, char* p_dest, char* p_src) {
  BTIF_TRACE_DEBUG("%s", __func__);
  tBTA_AV* av_src = (tBTA_AV*)p_src;
  tBTA_AV* av_dest = (tBTA_AV*)p_dest;

  maybe_non_aligned_memcpy(av_dest, av_src, sizeof(*av_src));
 switch (event) {
 case BTA_AV_META_MSG_EVT:
 if (av_src->meta_msg.p_data && av_src->meta_msg.len) {
        av_dest->meta_msg.p_data = (uint8_t*)osi_calloc(av_src->meta_msg.len);
        memcpy(av_dest->meta_msg.p_data, av_src->meta_msg.p_data,
               av_src->meta_msg.len);
 }

 if (av_src->meta_msg.p_msg) {
        av_dest->meta_msg.p_msg = (tAVRC_MSG*)osi_calloc(sizeof(tAVRC_MSG));
        memcpy(av_dest->meta_msg.p_msg, av_src->meta_msg.p_msg,
 sizeof(tAVRC_MSG));

        tAVRC_MSG* p_msg_src = av_src->meta_msg.p_msg;
        tAVRC_MSG* p_msg_dest = av_dest->meta_msg.p_msg;

 if ((p_msg_src->hdr.opcode == AVRC_OP_VENDOR) &&
 (p_msg_src->vendor.p_vendor_data && p_msg_src->vendor.vendor_len)) {
          p_msg_dest->vendor.p_vendor_data =
 (uint8_t*)osi_calloc(p_msg_src->vendor.vendor_len);

           memcpy(p_msg_dest->vendor.p_vendor_data,
                  p_msg_src->vendor.p_vendor_data, p_msg_src->vendor.vendor_len);
         }
        if ((p_msg_src->hdr.opcode == AVRC_OP_BROWSE) &&
            p_msg_src->browse.p_browse_data && p_msg_src->browse.browse_len) {
          p_msg_dest->browse.p_browse_data =
              (uint8_t*)osi_calloc(p_msg_src->browse.browse_len);
          memcpy(p_msg_dest->browse.p_browse_data,
                 p_msg_src->browse.p_browse_data, p_msg_src->browse.browse_len);
          android_errorWriteLog(0x534e4554, "109699112");
        }
       }
       break;
 
 default:
 break;
 }
}
