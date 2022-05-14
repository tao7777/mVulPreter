eXosip_init (struct eXosip_t *excontext)
{
  osip_t *osip;
  int i;

  memset (excontext, 0, sizeof (eXosip_t));

  excontext->dscp = 0x1A;

  snprintf (excontext->ipv4_for_gateway, 256, "%s", "217.12.3.11");
  snprintf (excontext->ipv6_for_gateway, 256, "%s", "2001:638:500:101:2e0:81ff:fe24:37c6");

#ifdef WIN32
  /* Initializing windows socket library */
  {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD (1, 1);
    i = WSAStartup (wVersionRequested, &wsaData);
    if (i != 0) {
      OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_WARNING, NULL, "eXosip: Unable to initialize WINSOCK, reason: %d\n", i));
      /* return -1; It might be already initilized?? */
    }
  }
#endif

  excontext->user_agent = osip_strdup ("eXosip/" EXOSIP_VERSION);
  if (excontext->user_agent == NULL)
    return OSIP_NOMEM;

  excontext->j_calls = NULL;
  excontext->j_stop_ua = 0;
#ifndef OSIP_MONOTHREAD
  excontext->j_thread = NULL;
#endif
  i = osip_list_init (&excontext->j_transactions);
  excontext->j_reg = NULL;

#ifndef OSIP_MONOTHREAD
#if !defined (_WIN32_WCE)
  excontext->j_cond = (struct osip_cond *) osip_cond_init ();
  if (excontext->j_cond == NULL) {
    osip_free (excontext->user_agent);
    excontext->user_agent = NULL;
    return OSIP_NOMEM;
  }
#endif

  excontext->j_mutexlock = (struct osip_mutex *) osip_mutex_init ();
  if (excontext->j_mutexlock == NULL) {
    osip_free (excontext->user_agent);
    excontext->user_agent = NULL;
#if !defined (_WIN32_WCE)
    osip_cond_destroy ((struct osip_cond *) excontext->j_cond);
    excontext->j_cond = NULL;
#endif
    return OSIP_NOMEM;
  }
#endif

  i = osip_init (&osip);
  if (i != 0) {
    OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL, "eXosip: Cannot initialize osip!\n"));
    return i;
  }

  osip_set_application_context (osip, &excontext);

  _eXosip_set_callbacks (osip);

  excontext->j_osip = osip;

#ifndef OSIP_MONOTHREAD
  /* open a TCP socket to wake up the application when needed. */
  excontext->j_socketctl = jpipe ();
  if (excontext->j_socketctl == NULL)
    return OSIP_UNDEFINED_ERROR;

  excontext->j_socketctl_event = jpipe ();
  if (excontext->j_socketctl_event == NULL)
    return OSIP_UNDEFINED_ERROR;
#endif

  /* To be changed in osip! */
  excontext->j_events = (osip_fifo_t *) osip_malloc (sizeof (osip_fifo_t));
  if (excontext->j_events == NULL)
    return OSIP_NOMEM;
  osip_fifo_init (excontext->j_events);

  excontext->use_rport = 1;
  excontext->dns_capabilities = 2;
  excontext->enable_dns_cache = 1;
  excontext->ka_interval = 17000;
  snprintf(excontext->ka_crlf, sizeof(excontext->ka_crlf), "\r\n\r\n");
  excontext->ka_options = 0;
   excontext->autoanswer_bye = 1;
   excontext->auto_masquerade_contact = 1;
   excontext->masquerade_via=0;
  excontext->use_ephemeral_port=1;
 
   return OSIP_SUCCESS;
 }
