int dmarc_process() {
    int sr, origin;             /* used in SPF section */
    int dmarc_spf_result  = 0;  /* stores spf into dmarc conn ctx */
    pdkim_signature *sig  = NULL;
    BOOL has_dmarc_record = TRUE;
    u_char **ruf; /* forensic report addressees, if called for */

  /* ACLs have "control=dmarc_disable_verify" */
  if (dmarc_disable_verify == TRUE)
  {
    dmarc_ar_header = dmarc_auth_results_header(from_header, NULL);
    return OK;
  }

  /* Store the header From: sender domain for this part of DMARC.
   * If there is no from_header struct, then it's likely this message
   * is locally generated and relying on fixups to add it.  Just skip
   * the entire DMARC system if we can't find a From: header....or if
   * there was a previous error.
   */
  if (from_header == NULL || dmarc_abort == TRUE)
     dmarc_abort = TRUE;
   else
   {
  uschar * errormsg;
  int dummy, domain;
  uschar * p;
  uschar saveend;

  parse_allow_group = TRUE;
  p = parse_find_address_end(from_header->text, FALSE);
  saveend = *p; *p = '\0';
  if ((header_from_sender = parse_extract_address(from_header->text, &errormsg,
                              &dummy, &dummy, &domain, FALSE)))
    header_from_sender += domain;
  *p = saveend;

  /* The opendmarc library extracts the domain from the email address, but
   * only try to store it if it's not empty.  Otherwise, skip out of DMARC. */
  if (!header_from_sender || (strcmp( CCS header_from_sender, "") == 0))
    dmarc_abort = TRUE;
  libdm_status = dmarc_abort ?
    DMARC_PARSE_OKAY :
    opendmarc_policy_store_from_domain(dmarc_pctx, header_from_sender);
  if (libdm_status != DMARC_PARSE_OKAY)
     {
      log_write(0, LOG_MAIN|LOG_PANIC,
                "failure to store header From: in DMARC: %s, header was '%s'",
                opendmarc_policy_status_to_str(libdm_status), from_header->text);
       dmarc_abort = TRUE;
     }
   }
    /* Use the envelope sender domain for this part of DMARC */
    spf_sender_domain = expand_string(US"$sender_address_domain");
    if ( spf_response == NULL )
    {
      /* No spf data means null envelope sender so generate a domain name
       * from the sender_helo_name  */
      if (spf_sender_domain == NULL)
      {
        spf_sender_domain = sender_helo_name;
        log_write(0, LOG_MAIN, "DMARC using synthesized SPF sender domain = %s\n",
                               spf_sender_domain);
        DEBUG(D_receive)
          debug_printf("DMARC using synthesized SPF sender domain = %s\n", spf_sender_domain);
      }
      dmarc_spf_result = DMARC_POLICY_SPF_OUTCOME_NONE;
      dmarc_spf_ares_result = ARES_RESULT_UNKNOWN;
      origin = DMARC_POLICY_SPF_ORIGIN_HELO;
      spf_human_readable = US"";
    }
    else
    {
      sr = spf_response->result;
      dmarc_spf_result = (sr == SPF_RESULT_NEUTRAL)  ? DMARC_POLICY_SPF_OUTCOME_NONE :
                         (sr == SPF_RESULT_PASS)     ? DMARC_POLICY_SPF_OUTCOME_PASS :
                         (sr == SPF_RESULT_FAIL)     ? DMARC_POLICY_SPF_OUTCOME_FAIL :
                         (sr == SPF_RESULT_SOFTFAIL) ? DMARC_POLICY_SPF_OUTCOME_TMPFAIL :
                         DMARC_POLICY_SPF_OUTCOME_NONE;
      dmarc_spf_ares_result = (sr == SPF_RESULT_NEUTRAL)   ? ARES_RESULT_NEUTRAL :
                              (sr == SPF_RESULT_PASS)      ? ARES_RESULT_PASS :
                              (sr == SPF_RESULT_FAIL)      ? ARES_RESULT_FAIL :
                              (sr == SPF_RESULT_SOFTFAIL)  ? ARES_RESULT_SOFTFAIL :
                              (sr == SPF_RESULT_NONE)      ? ARES_RESULT_NONE :
                              (sr == SPF_RESULT_TEMPERROR) ? ARES_RESULT_TEMPERROR :
                              (sr == SPF_RESULT_PERMERROR) ? ARES_RESULT_PERMERROR :
                              ARES_RESULT_UNKNOWN;
      origin = DMARC_POLICY_SPF_ORIGIN_MAILFROM;
      spf_human_readable = (uschar *)spf_response->header_comment;
      DEBUG(D_receive)
        debug_printf("DMARC using SPF sender domain = %s\n", spf_sender_domain);
    }
    if (strcmp( CCS spf_sender_domain, "") == 0)
      dmarc_abort = TRUE;
    if (dmarc_abort == FALSE)
    {
      libdm_status = opendmarc_policy_store_spf(dmarc_pctx, spf_sender_domain,
                                                dmarc_spf_result, origin, spf_human_readable);
      if (libdm_status != DMARC_PARSE_OKAY)
        log_write(0, LOG_MAIN|LOG_PANIC, "failure to store spf for DMARC: %s",
                             opendmarc_policy_status_to_str(libdm_status));
    }

    /* Now we cycle through the dkim signature results and put into
     * the opendmarc context, further building the DMARC reply.  */
    sig = dkim_signatures;
    dkim_history_buffer = US"";
    while (sig != NULL)
    {
      int dkim_result, dkim_ares_result, vs, ves;
      vs  = sig->verify_status;
      ves = sig->verify_ext_status;
      dkim_result = ( vs == PDKIM_VERIFY_PASS ) ? DMARC_POLICY_DKIM_OUTCOME_PASS :
                    ( vs == PDKIM_VERIFY_FAIL ) ? DMARC_POLICY_DKIM_OUTCOME_FAIL :
                    ( vs == PDKIM_VERIFY_INVALID ) ? DMARC_POLICY_DKIM_OUTCOME_TMPFAIL :
                    DMARC_POLICY_DKIM_OUTCOME_NONE;
      libdm_status = opendmarc_policy_store_dkim(dmarc_pctx, (uschar *)sig->domain,
        	                                 dkim_result, US"");
      DEBUG(D_receive)
        debug_printf("DMARC adding DKIM sender domain = %s\n", sig->domain);
      if (libdm_status != DMARC_PARSE_OKAY)
        log_write(0, LOG_MAIN|LOG_PANIC, "failure to store dkim (%s) for DMARC: %s",
        		     sig->domain, opendmarc_policy_status_to_str(libdm_status));

      dkim_ares_result = ( vs == PDKIM_VERIFY_PASS )    ? ARES_RESULT_PASS :
        	              ( vs == PDKIM_VERIFY_FAIL )    ? ARES_RESULT_FAIL :
        	              ( vs == PDKIM_VERIFY_NONE )    ? ARES_RESULT_NONE :
        	              ( vs == PDKIM_VERIFY_INVALID ) ?
                           ( ves == PDKIM_VERIFY_INVALID_PUBKEY_UNAVAILABLE ? ARES_RESULT_PERMERROR :
                             ves == PDKIM_VERIFY_INVALID_BUFFER_SIZE        ? ARES_RESULT_PERMERROR :
                             ves == PDKIM_VERIFY_INVALID_PUBKEY_PARSING     ? ARES_RESULT_PERMERROR :
                             ARES_RESULT_UNKNOWN ) :
                          ARES_RESULT_UNKNOWN;
      dkim_history_buffer = string_sprintf("%sdkim %s %d\n", dkim_history_buffer,
                                                             sig->domain, dkim_ares_result);
      sig = sig->next;
    }
    libdm_status = opendmarc_policy_query_dmarc(dmarc_pctx, US"");
    switch (libdm_status)
    {
      case DMARC_DNS_ERROR_NXDOMAIN:
      case DMARC_DNS_ERROR_NO_RECORD:
        DEBUG(D_receive)
          debug_printf("DMARC no record found for %s\n", header_from_sender);
        has_dmarc_record = FALSE;
        break;
      case DMARC_PARSE_OKAY:
        DEBUG(D_receive)
          debug_printf("DMARC record found for %s\n", header_from_sender);
        break;
      case DMARC_PARSE_ERROR_BAD_VALUE:
        DEBUG(D_receive)
          debug_printf("DMARC record parse error for %s\n", header_from_sender);
        has_dmarc_record = FALSE;
        break;
      default:
        /* everything else, skip dmarc */
        DEBUG(D_receive)
          debug_printf("DMARC skipping (%d), unsure what to do with %s",
                        libdm_status, from_header->text);
        has_dmarc_record = FALSE;
        break;
    }
    /* Can't use exim's string manipulation functions so allocate memory
     * for libopendmarc using its max hostname length definition. */
    uschar *dmarc_domain = (uschar *)calloc(DMARC_MAXHOSTNAMELEN, sizeof(uschar));
    libdm_status = opendmarc_policy_fetch_utilized_domain(dmarc_pctx, dmarc_domain,
        	                                          DMARC_MAXHOSTNAMELEN-1);
    dmarc_used_domain = string_copy(dmarc_domain);
    free(dmarc_domain);
    if (libdm_status != DMARC_PARSE_OKAY)
    {
      log_write(0, LOG_MAIN|LOG_PANIC, "failure to read domainname used for DMARC lookup: %s",
                                       opendmarc_policy_status_to_str(libdm_status));
    }
    libdm_status = opendmarc_get_policy_to_enforce(dmarc_pctx);
    dmarc_policy = libdm_status;
    switch(libdm_status)
    {
      case DMARC_POLICY_ABSENT:     /* No DMARC record found */
        dmarc_status = US"norecord";
        dmarc_pass_fail = US"none";
        dmarc_status_text = US"No DMARC record";
        action = DMARC_RESULT_ACCEPT;
        break;
      case DMARC_FROM_DOMAIN_ABSENT:    /* No From: domain */
        dmarc_status = US"nofrom";
        dmarc_pass_fail = US"temperror";
        dmarc_status_text = US"No From: domain found";
        action = DMARC_RESULT_ACCEPT;
        break;
      case DMARC_POLICY_NONE:       /* Accept and report */
        dmarc_status = US"none";
        dmarc_pass_fail = US"none";
        dmarc_status_text = US"None, Accept";
        action = DMARC_RESULT_ACCEPT;
        break;
      case DMARC_POLICY_PASS:       /* Explicit accept */
        dmarc_status = US"accept";
        dmarc_pass_fail = US"pass";
        dmarc_status_text = US"Accept";
        action = DMARC_RESULT_ACCEPT;
        break;
      case DMARC_POLICY_REJECT:       /* Explicit reject */
        dmarc_status = US"reject";
        dmarc_pass_fail = US"fail";
        dmarc_status_text = US"Reject";
        action = DMARC_RESULT_REJECT;
        break;
      case DMARC_POLICY_QUARANTINE:       /* Explicit quarantine */
        dmarc_status = US"quarantine";
        dmarc_pass_fail = US"fail";
        dmarc_status_text = US"Quarantine";
        action = DMARC_RESULT_QUARANTINE;
        break;
      default:
        dmarc_status = US"temperror";
        dmarc_pass_fail = US"temperror";
        dmarc_status_text = US"Internal Policy Error";
        action = DMARC_RESULT_TEMPFAIL;
        break;
    }

    libdm_status = opendmarc_policy_fetch_alignment(dmarc_pctx, &da, &sa);
    if (libdm_status != DMARC_PARSE_OKAY)
    {
      log_write(0, LOG_MAIN|LOG_PANIC, "failure to read DMARC alignment: %s",
                                       opendmarc_policy_status_to_str(libdm_status));
    }

    if (has_dmarc_record == TRUE)
    {
      log_write(0, LOG_MAIN, "DMARC results: spf_domain=%s dmarc_domain=%s "
                             "spf_align=%s dkim_align=%s enforcement='%s'",
                             spf_sender_domain, dmarc_used_domain,
                             (sa==DMARC_POLICY_SPF_ALIGNMENT_PASS) ?"yes":"no",
                             (da==DMARC_POLICY_DKIM_ALIGNMENT_PASS)?"yes":"no",
                             dmarc_status_text);
      history_file_status = dmarc_write_history_file();
      /* Now get the forensic reporting addresses, if any */
      ruf = opendmarc_policy_fetch_ruf(dmarc_pctx, NULL, 0, 1);
      dmarc_send_forensic_report(ruf);
    }
  }
