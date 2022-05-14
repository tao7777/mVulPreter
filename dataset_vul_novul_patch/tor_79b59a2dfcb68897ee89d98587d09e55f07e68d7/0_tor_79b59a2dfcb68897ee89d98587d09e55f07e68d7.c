connection_exit_begin_conn(cell_t *cell, circuit_t *circ)
{
  edge_connection_t *n_stream;
  relay_header_t rh;
   char *address = NULL;
   uint16_t port = 0;
   or_circuit_t *or_circ = NULL;
  origin_circuit_t *origin_circ = NULL;
  crypt_path_t *layer_hint = NULL;
   const or_options_t *options = get_options();
   begin_cell_t bcell;
   int rv;
   uint8_t end_reason=0;
 
   assert_circuit_ok(circ);
  if (!CIRCUIT_IS_ORIGIN(circ)) {
     or_circ = TO_OR_CIRCUIT(circ);
  } else {
    tor_assert(circ->purpose == CIRCUIT_PURPOSE_S_REND_JOINED);
    origin_circ = TO_ORIGIN_CIRCUIT(circ);
    layer_hint = origin_circ->cpath->prev;
  }
 
   relay_header_unpack(&rh, cell->payload);
   if (rh.length > RELAY_PAYLOAD_SIZE)
    return -END_CIRC_REASON_TORPROTOCOL;

  /* Note: we have to use relay_send_command_from_edge here, not
   * connection_edge_end or connection_edge_send_command, since those require
   * that we have a stream connected to a circuit, and we don't connect to a
   * circuit until we have a pending/successful resolve. */

  if (!server_mode(options) &&
      circ->purpose != CIRCUIT_PURPOSE_S_REND_JOINED) {
    log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
           "Relay begin cell at non-server. Closing.");
    relay_send_end_cell_from_edge(rh.stream_id, circ,
                                  END_STREAM_REASON_EXITPOLICY, NULL);
    return 0;
  }

  rv = begin_cell_parse(cell, &bcell, &end_reason);
  if (rv < -1) {
     return -END_CIRC_REASON_TORPROTOCOL;
   } else if (rv == -1) {
     tor_free(bcell.address);
    relay_send_end_cell_from_edge(rh.stream_id, circ, end_reason, layer_hint);
     return 0;
   }
 
  if (! bcell.is_begindir) {
    /* Steal reference */
    address = bcell.address;
    port = bcell.port;

    if (or_circ && or_circ->p_chan) {
      if (!options->AllowSingleHopExits &&
           (or_circ->is_first_hop ||
            (!connection_or_digest_is_known_relay(
                or_circ->p_chan->identity_digest) &&
          should_refuse_unknown_exits(options)))) {
        /* Don't let clients use us as a single-hop proxy, unless the user
         * has explicitly allowed that in the config. It attracts attackers
         * and users who'd be better off with, well, single-hop proxies.
         */
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
               "Attempt by %s to open a stream %s. Closing.",
               safe_str(channel_get_canonical_remote_descr(or_circ->p_chan)),
               or_circ->is_first_hop ? "on first hop of circuit" :
                                       "from unknown relay");
        relay_send_end_cell_from_edge(rh.stream_id, circ,
                                      or_circ->is_first_hop ?
                                        END_STREAM_REASON_TORPROTOCOL :
                                        END_STREAM_REASON_MISC,
                                      NULL);
        tor_free(address);
        return 0;
      }
    }
  } else if (rh.command == RELAY_COMMAND_BEGIN_DIR) {
     if (!directory_permits_begindir_requests(options) ||
         circ->purpose != CIRCUIT_PURPOSE_OR) {
       relay_send_end_cell_from_edge(rh.stream_id, circ,
                                  END_STREAM_REASON_NOTDIRECTORY, layer_hint);
       return 0;
     }
     /* Make sure to get the 'real' address of the previous hop: the
     * caller might want to know whether the remote IP address has changed,
     * and we might already have corrected base_.addr[ess] for the relay's
     * canonical IP address. */
    if (or_circ && or_circ->p_chan)
      address = tor_strdup(channel_get_actual_remote_address(or_circ->p_chan));
    else
      address = tor_strdup("127.0.0.1");
    port = 1; /* XXXX This value is never actually used anywhere, and there
               * isn't "really" a connection here.  But we
               * need to set it to something nonzero. */
   } else {
     log_warn(LD_BUG, "Got an unexpected command %d", (int)rh.command);
     relay_send_end_cell_from_edge(rh.stream_id, circ,
                                  END_STREAM_REASON_INTERNAL, layer_hint);
     return 0;
   }
 
  if (! options->IPv6Exit) {
    /* I don't care if you prefer IPv6; I can't give you any. */
    bcell.flags &= ~BEGIN_FLAG_IPV6_PREFERRED;
    /* If you don't want IPv4, I can't help. */
     if (bcell.flags & BEGIN_FLAG_IPV4_NOT_OK) {
       tor_free(address);
       relay_send_end_cell_from_edge(rh.stream_id, circ,
                                    END_STREAM_REASON_EXITPOLICY, layer_hint);
       return 0;
     }
   }

  log_debug(LD_EXIT,"Creating new exit connection.");
  /* The 'AF_INET' here is temporary; we might need to change it later in
   * connection_exit_connect(). */
  n_stream = edge_connection_new(CONN_TYPE_EXIT, AF_INET);

  /* Remember the tunneled request ID in the new edge connection, so that
   * we can measure download times. */
  n_stream->dirreq_id = circ->dirreq_id;

  n_stream->base_.purpose = EXIT_PURPOSE_CONNECT;
  n_stream->begincell_flags = bcell.flags;
  n_stream->stream_id = rh.stream_id;
  n_stream->base_.port = port;
  /* leave n_stream->s at -1, because it's not yet valid */
  n_stream->package_window = STREAMWINDOW_START;
   n_stream->deliver_window = STREAMWINDOW_START;
 
   if (circ->purpose == CIRCUIT_PURPOSE_S_REND_JOINED) {
    tor_assert(origin_circ);
     log_info(LD_REND,"begin is for rendezvous. configuring stream.");
     n_stream->base_.address = tor_strdup("(rendezvous)");
     n_stream->base_.state = EXIT_CONN_STATE_CONNECTING;
    n_stream->rend_data = rend_data_dup(origin_circ->rend_data);
    tor_assert(connection_edge_is_rendezvous_stream(n_stream));
    assert_circuit_ok(circ);

    const int r = rend_service_set_connection_addr_port(n_stream, origin_circ);
    if (r < 0) {
      log_info(LD_REND,"Didn't find rendezvous service (port %d)",
               n_stream->base_.port);
      /* Send back reason DONE because we want to make hidden service port
       * scanning harder thus instead of returning that the exit policy
       * didn't match, which makes it obvious that the port is closed,
       * return DONE and kill the circuit. That way, a user (malicious or
       * not) needs one circuit per bad port unless it matches the policy of
        * the hidden service. */
       relay_send_end_cell_from_edge(rh.stream_id, circ,
                                     END_STREAM_REASON_DONE,
                                    layer_hint);
       connection_free(TO_CONN(n_stream));
       tor_free(address);
 
      /* Drop the circuit here since it might be someone deliberately
       * scanning the hidden service ports. Note that this mitigates port
       * scanning by adding more work on the attacker side to successfully
       * scan but does not fully solve it. */
      if (r < -1)
        return END_CIRC_AT_ORIGIN;
      else
        return 0;
    }
    assert_circuit_ok(circ);
    log_debug(LD_REND,"Finished assigning addr/port");
    n_stream->cpath_layer = origin_circ->cpath->prev; /* link it */

    /* add it into the linked list of p_streams on this circuit */
    n_stream->next_stream = origin_circ->p_streams;
    n_stream->on_circuit = circ;
    origin_circ->p_streams = n_stream;
    assert_circuit_ok(circ);

    origin_circ->rend_data->nr_streams++;

    connection_exit_connect(n_stream);

    /* For path bias: This circuit was used successfully */
    pathbias_mark_use_success(origin_circ);

    tor_free(address);
    return 0;
  }
  tor_strlower(address);
  n_stream->base_.address = address;
  n_stream->base_.state = EXIT_CONN_STATE_RESOLVEFAILED;
  /* default to failed, change in dns_resolve if it turns out not to fail */

  if (we_are_hibernating()) {
    relay_send_end_cell_from_edge(rh.stream_id, circ,
                                  END_STREAM_REASON_HIBERNATING, NULL);
    connection_free(TO_CONN(n_stream));
    return 0;
  }

  n_stream->on_circuit = circ;

  if (rh.command == RELAY_COMMAND_BEGIN_DIR) {
    tor_addr_t tmp_addr;
    tor_assert(or_circ);
    if (or_circ->p_chan &&
        channel_get_addr_if_possible(or_circ->p_chan, &tmp_addr)) {
      tor_addr_copy(&n_stream->base_.addr, &tmp_addr);
    }
    return connection_exit_connect_dir(n_stream);
  }

  log_debug(LD_EXIT,"about to start the dns_resolve().");

  /* send it off to the gethostbyname farm */
  switch (dns_resolve(n_stream)) {
    case 1: /* resolve worked; now n_stream is attached to circ. */
      assert_circuit_ok(circ);
      log_debug(LD_EXIT,"about to call connection_exit_connect().");
      connection_exit_connect(n_stream);
      return 0;
    case -1: /* resolve failed */
      relay_send_end_cell_from_edge(rh.stream_id, circ,
                                    END_STREAM_REASON_RESOLVEFAILED, NULL);
      /* n_stream got freed. don't touch it. */
      break;
    case 0: /* resolve added to pending list */
      assert_circuit_ok(circ);
      break;
  }
  return 0;
}
