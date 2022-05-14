new_msg_register_event (u_int32_t seqnum, struct lsa_filter_type *filter)
{
  u_char buf[OSPF_API_MAX_MSG_SIZE];
  struct msg_register_event *emsg;
  int len;

  emsg = (struct msg_register_event *) buf;
  len = sizeof (struct msg_register_event) +
    filter->num_areas * sizeof (struct in_addr);
   emsg->filter.typemask = htons (filter->typemask);
   emsg->filter.origin = filter->origin;
   emsg->filter.num_areas = filter->num_areas;
  if (len > sizeof (buf))
    len = sizeof(buf);
  /* API broken - missing memcpy to fill data */
   return msg_new (MSG_REGISTER_EVENT, emsg, seqnum, len);
 }
