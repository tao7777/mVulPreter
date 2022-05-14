static void sdp_copy_raw_data(tCONN_CB* p_ccb, bool offset) {
 unsigned int cpy_len, rem_len;
 uint32_t list_len;
 uint8_t* p;
 uint8_t type;

#if (SDP_DEBUG_RAW == TRUE)
 uint8_t num_array[SDP_MAX_LIST_BYTE_COUNT];
 uint32_t i;

 for (i = 0; i < p_ccb->list_len; i++) {
    snprintf((char*)&num_array[i * 2], sizeof(num_array) - i * 2, "%02X",
 (uint8_t)(p_ccb->rsp_list[i]));
 }
  SDP_TRACE_WARNING("result :%s", num_array);
#endif

 if (p_ccb->p_db->raw_data) {
    cpy_len = p_ccb->p_db->raw_size - p_ccb->p_db->raw_used;
    list_len = p_ccb->list_len;

     p = &p_ccb->rsp_list[0];
 
     if (offset) {
      cpy_len -= 1;
       type = *p++;
      uint8_t* old_p = p;
       p = sdpu_get_len_from_type(p, type, &list_len);
      if ((int)cpy_len < (p - old_p)) {
        SDP_TRACE_WARNING("%s: no bytes left for data", __func__);
        return;
      }
      cpy_len -= (p - old_p);
     }
     if (list_len < cpy_len) {
       cpy_len = list_len;
 }
    rem_len = SDP_MAX_LIST_BYTE_COUNT - (unsigned int)(p - &p_ccb->rsp_list[0]);
 if (cpy_len > rem_len) {
      SDP_TRACE_WARNING("rem_len :%d less than cpy_len:%d", rem_len, cpy_len);
      cpy_len = rem_len;
 }
    SDP_TRACE_WARNING(
 "%s: list_len:%d cpy_len:%d p:%p p_ccb:%p p_db:%p raw_size:%d "
 "raw_used:%d raw_data:%p",
        __func__, list_len, cpy_len, p, p_ccb, p_ccb->p_db,
        p_ccb->p_db->raw_size, p_ccb->p_db->raw_used, p_ccb->p_db->raw_data);
    memcpy(&p_ccb->p_db->raw_data[p_ccb->p_db->raw_used], p, cpy_len);
    p_ccb->p_db->raw_used += cpy_len;
 }
}
