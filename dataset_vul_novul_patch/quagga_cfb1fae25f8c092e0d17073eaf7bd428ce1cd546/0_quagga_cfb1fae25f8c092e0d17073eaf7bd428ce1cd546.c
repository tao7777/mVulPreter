rtadv_read (struct thread *thread)
{
  int sock;
  int len;
  u_char buf[RTADV_MSG_SIZE];
  struct sockaddr_in6 from;
  ifindex_t ifindex = 0;
  int hoplimit = -1;
  struct zebra_vrf *zvrf = THREAD_ARG (thread);

  sock = THREAD_FD (thread);
  zvrf->rtadv.ra_read = NULL;

   /* Register myself. */
   rtadv_event (zvrf, RTADV_READ, sock);
 
  len = rtadv_recv_packet (sock, buf, sizeof (buf), &from, &ifindex, &hoplimit);
 
   if (len < 0) 
     {
      zlog_warn ("router solicitation recv failed: %s.", safe_strerror (errno));
      return len;
    }

  rtadv_process_packet (buf, (unsigned)len, ifindex, hoplimit, zvrf->vrf_id);

  return 0;
}
