dissect_rpcap_packet (tvbuff_t *tvb, packet_info *pinfo, proto_tree *top_tree,
                      proto_tree *parent_tree, gint offset, proto_item *top_item)
{
  proto_tree *tree;
  proto_item *ti;
  nstime_t ts;
   tvbuff_t *new_tvb;
   guint caplen, len, frame_no;
   gint reported_length_remaining;
  struct eth_phdr eth;
  void *phdr;
 
   ti = proto_tree_add_item (parent_tree, hf_packet, tvb, offset, 20, ENC_NA);
   tree = proto_item_add_subtree (ti, ett_packet);

  ts.secs = tvb_get_ntohl (tvb, offset);
  ts.nsecs = tvb_get_ntohl (tvb, offset + 4) * 1000;
  proto_tree_add_time(tree, hf_timestamp, tvb, offset, 8, &ts);
  offset += 8;

  caplen = tvb_get_ntohl (tvb, offset);
  ti = proto_tree_add_item (tree, hf_caplen, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  len = tvb_get_ntohl (tvb, offset);
  proto_tree_add_item (tree, hf_len, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  frame_no = tvb_get_ntohl (tvb, offset);
  proto_tree_add_item (tree, hf_npkt, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_item_append_text (ti, ", Frame %u", frame_no);
  proto_item_append_text (top_item, " Frame %u", frame_no);

  /*
   * reported_length_remaining should not be -1, as offset is at
   * most right past the end of the available data in the packet.
   */
  reported_length_remaining = tvb_reported_length_remaining (tvb, offset);
  if (caplen > (guint)reported_length_remaining) {
    expert_add_info(pinfo, ti, &ei_caplen_too_big);
    return;
  }
 
   new_tvb = tvb_new_subset (tvb, offset, caplen, len);
   if (decode_content && linktype != WTAP_ENCAP_UNKNOWN) {
    switch (linktype) {

    case WTAP_ENCAP_ETHERNET:
      eth.fcs_len = -1;    /* Unknown whether we have an FCS */
      phdr = &eth;
      break;

    default:
      phdr = NULL;
      break;
    }
    dissector_try_uint_new(wtap_encap_dissector_table, linktype, new_tvb, pinfo, top_tree, TRUE, phdr);
 
     if (!info_added) {
       /* Only indicate when not added before */
      /* Indicate RPCAP in the protocol column */
      col_prepend_fence_fstr(pinfo->cinfo, COL_PROTOCOL, "R|");

      /* Indicate RPCAP in the info column */
      col_prepend_fence_fstr (pinfo->cinfo, COL_INFO, "Remote | ");
      info_added = TRUE;
      register_frame_end_routine(pinfo, rpcap_frame_end);
    }
  } else {
    if (linktype == WTAP_ENCAP_UNKNOWN) {
      proto_item_append_text (ti, ", Unknown link-layer type");
    }
    call_dissector (data_handle, new_tvb, pinfo, top_tree);
  }
}
