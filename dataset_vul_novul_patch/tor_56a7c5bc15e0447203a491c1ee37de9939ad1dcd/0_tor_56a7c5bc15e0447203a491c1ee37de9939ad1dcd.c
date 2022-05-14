connection_edge_process_relay_cell(cell_t *cell, circuit_t *circ,
                                   edge_connection_t *conn,
                                   crypt_path_t *layer_hint)
{
  static int num_seen=0;
  relay_header_t rh;
  unsigned domain = layer_hint?LD_APP:LD_EXIT;
  int reason;
  int optimistic_data = 0; /* Set to 1 if we receive data on a stream
                            * that's in the EXIT_CONN_STATE_RESOLVING
                            * or EXIT_CONN_STATE_CONNECTING states. */

  tor_assert(cell);
  tor_assert(circ);

  relay_header_unpack(&rh, cell->payload);
  num_seen++;
  log_debug(domain, "Now seen %d relay cells here (command %d, stream %d).",
            num_seen, rh.command, rh.stream_id);

  if (rh.length > RELAY_PAYLOAD_SIZE) {
    log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
           "Relay cell length field too long. Closing circuit.");
    return - END_CIRC_REASON_TORPROTOCOL;
  }

  if (rh.stream_id == 0) {
    switch (rh.command) {
      case RELAY_COMMAND_BEGIN:
      case RELAY_COMMAND_CONNECTED:
      case RELAY_COMMAND_DATA:
      case RELAY_COMMAND_END:
      case RELAY_COMMAND_RESOLVE:
      case RELAY_COMMAND_RESOLVED:
      case RELAY_COMMAND_BEGIN_DIR:
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL, "Relay command %d with zero "
               "stream_id. Dropping.", (int)rh.command);
        return 0;
      default:
        ;
    }
  }

  /* either conn is NULL, in which case we've got a control cell, or else
   * conn points to the recognized stream. */

  if (conn && !connection_state_is_open(TO_CONN(conn))) {
    if (conn->base_.type == CONN_TYPE_EXIT &&
        (conn->base_.state == EXIT_CONN_STATE_CONNECTING ||
         conn->base_.state == EXIT_CONN_STATE_RESOLVING) &&
        rh.command == RELAY_COMMAND_DATA) {
      /* Allow DATA cells to be delivered to an exit node in state
       * EXIT_CONN_STATE_CONNECTING or EXIT_CONN_STATE_RESOLVING.
       * This speeds up HTTP, for example. */
      optimistic_data = 1;
    } else {
      return connection_edge_process_relay_cell_not_open(
               &rh, cell, circ, conn, layer_hint);
    }
  }

  switch (rh.command) {
    case RELAY_COMMAND_DROP:
      return 0;
    case RELAY_COMMAND_BEGIN:
    case RELAY_COMMAND_BEGIN_DIR:
      if (layer_hint &&
          circ->purpose != CIRCUIT_PURPOSE_S_REND_JOINED) {
        log_fn(LOG_PROTOCOL_WARN, LD_APP,
               "Relay begin request unsupported at AP. Dropping.");
        return 0;
      }
      if (circ->purpose == CIRCUIT_PURPOSE_S_REND_JOINED &&
          layer_hint != TO_ORIGIN_CIRCUIT(circ)->cpath->prev) {
        log_fn(LOG_PROTOCOL_WARN, LD_APP,
               "Relay begin request to Hidden Service "
               "from intermediary node. Dropping.");
        return 0;
      }
      if (conn) {
        log_fn(LOG_PROTOCOL_WARN, domain,
                "Begin cell for known stream. Dropping.");
         return 0;
       }
      if (rh.command == RELAY_COMMAND_BEGIN_DIR &&
          circ->purpose != CIRCUIT_PURPOSE_S_REND_JOINED) {
         /* Assign this circuit and its app-ward OR connection a unique ID,
          * so that we can measure download times. The local edge and dir
          * connection will be assigned the same ID when they are created
         * and linked. */
        static uint64_t next_id = 0;
        circ->dirreq_id = ++next_id;
        TO_OR_CIRCUIT(circ)->p_chan->dirreq_id = circ->dirreq_id;
      }

      return connection_exit_begin_conn(cell, circ);
    case RELAY_COMMAND_DATA:
      ++stats_n_data_cells_received;
      if (( layer_hint && --layer_hint->deliver_window < 0) ||
          (!layer_hint && --circ->deliver_window < 0)) {
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
               "(relay data) circ deliver_window below 0. Killing.");
        if (conn) {
          /* XXXX Do we actually need to do this?  Will killing the circuit
           * not send an END and mark the stream for close as appropriate? */
          connection_edge_end(conn, END_STREAM_REASON_TORPROTOCOL);
          connection_mark_for_close(TO_CONN(conn));
        }
        return -END_CIRC_REASON_TORPROTOCOL;
      }
      log_debug(domain,"circ deliver_window now %d.", layer_hint ?
                layer_hint->deliver_window : circ->deliver_window);

      circuit_consider_sending_sendme(circ, layer_hint);

      if (!conn) {
        log_info(domain,"data cell dropped, unknown stream (streamid %d).",
                 rh.stream_id);
        return 0;
      }

      if (--conn->deliver_window < 0) { /* is it below 0 after decrement? */
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
               "(relay data) conn deliver_window below 0. Killing.");
        return -END_CIRC_REASON_TORPROTOCOL;
      }

      stats_n_data_bytes_received += rh.length;
      connection_write_to_buf((char*)(cell->payload + RELAY_HEADER_SIZE),
                              rh.length, TO_CONN(conn));

      if (!optimistic_data) {
        /* Only send a SENDME if we're not getting optimistic data; otherwise
         * a SENDME could arrive before the CONNECTED.
         */
        connection_edge_consider_sending_sendme(conn);
      }

      return 0;
    case RELAY_COMMAND_END:
      reason = rh.length > 0 ?
        get_uint8(cell->payload+RELAY_HEADER_SIZE) : END_STREAM_REASON_MISC;
      if (!conn) {
        log_info(domain,"end cell (%s) dropped, unknown stream.",
                 stream_end_reason_to_string(reason));
        return 0;
      }
/* XXX add to this log_fn the exit node's nickname? */
      log_info(domain,TOR_SOCKET_T_FORMAT": end cell (%s) for stream %d. "
               "Removing stream.",
               conn->base_.s,
               stream_end_reason_to_string(reason),
               conn->stream_id);
      if (conn->base_.type == CONN_TYPE_AP) {
        entry_connection_t *entry_conn = EDGE_TO_ENTRY_CONN(conn);
        if (entry_conn->socks_request &&
            !entry_conn->socks_request->has_finished)
          log_warn(LD_BUG,
                   "open stream hasn't sent socks answer yet? Closing.");
      }
      /* We just *got* an end; no reason to send one. */
      conn->edge_has_sent_end = 1;
      if (!conn->end_reason)
        conn->end_reason = reason | END_STREAM_REASON_FLAG_REMOTE;
      if (!conn->base_.marked_for_close) {
        /* only mark it if not already marked. it's possible to
         * get the 'end' right around when the client hangs up on us. */
        connection_mark_and_flush(TO_CONN(conn));
      }
      return 0;
    case RELAY_COMMAND_EXTEND:
    case RELAY_COMMAND_EXTEND2: {
      static uint64_t total_n_extend=0, total_nonearly=0;
      total_n_extend++;
      if (rh.stream_id) {
        log_fn(LOG_PROTOCOL_WARN, domain,
               "'extend' cell received for non-zero stream. Dropping.");
        return 0;
      }
      if (cell->command != CELL_RELAY_EARLY &&
          !networkstatus_get_param(NULL,"AllowNonearlyExtend",0,0,1)) {
#define EARLY_WARNING_INTERVAL 3600
        static ratelim_t early_warning_limit =
          RATELIM_INIT(EARLY_WARNING_INTERVAL);
        char *m;
        if (cell->command == CELL_RELAY) {
          ++total_nonearly;
          if ((m = rate_limit_log(&early_warning_limit, approx_time()))) {
            double percentage = ((double)total_nonearly)/total_n_extend;
            percentage *= 100;
            log_fn(LOG_PROTOCOL_WARN, domain, "EXTEND cell received, "
                   "but not via RELAY_EARLY. Dropping.%s", m);
            log_fn(LOG_PROTOCOL_WARN, domain, "  (We have dropped %.02f%% of "
                   "all EXTEND cells for this reason)", percentage);
            tor_free(m);
          }
        } else {
          log_fn(LOG_WARN, domain,
                 "EXTEND cell received, in a cell with type %d! Dropping.",
                 cell->command);
        }
        return 0;
      }
      return circuit_extend(cell, circ);
    }
    case RELAY_COMMAND_EXTENDED:
    case RELAY_COMMAND_EXTENDED2:
      if (!layer_hint) {
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
               "'extended' unsupported at non-origin. Dropping.");
        return 0;
      }
      log_debug(domain,"Got an extended cell! Yay.");
      {
        extended_cell_t extended_cell;
        if (extended_cell_parse(&extended_cell, rh.command,
                        (const uint8_t*)cell->payload+RELAY_HEADER_SIZE,
                        rh.length)<0) {
          log_warn(LD_PROTOCOL,
                   "Can't parse EXTENDED cell; killing circuit.");
          return -END_CIRC_REASON_TORPROTOCOL;
        }
        if ((reason = circuit_finish_handshake(TO_ORIGIN_CIRCUIT(circ),
                                         &extended_cell.created_cell)) < 0) {
          log_warn(domain,"circuit_finish_handshake failed.");
          return reason;
        }
      }
      if ((reason=circuit_send_next_onion_skin(TO_ORIGIN_CIRCUIT(circ)))<0) {
        log_info(domain,"circuit_send_next_onion_skin() failed.");
        return reason;
      }
      return 0;
    case RELAY_COMMAND_TRUNCATE:
      if (layer_hint) {
        log_fn(LOG_PROTOCOL_WARN, LD_APP,
               "'truncate' unsupported at origin. Dropping.");
        return 0;
      }
      if (circ->n_hop) {
        if (circ->n_chan)
          log_warn(LD_BUG, "n_chan and n_hop set on the same circuit!");
        extend_info_free(circ->n_hop);
        circ->n_hop = NULL;
        tor_free(circ->n_chan_create_cell);
        circuit_set_state(circ, CIRCUIT_STATE_OPEN);
      }
      if (circ->n_chan) {
        uint8_t trunc_reason = get_uint8(cell->payload + RELAY_HEADER_SIZE);
        circuit_clear_cell_queue(circ, circ->n_chan);
        channel_send_destroy(circ->n_circ_id, circ->n_chan,
                             trunc_reason);
        circuit_set_n_circid_chan(circ, 0, NULL);
      }
      log_debug(LD_EXIT, "Processed 'truncate', replying.");
      {
        char payload[1];
        payload[0] = (char)END_CIRC_REASON_REQUESTED;
        relay_send_command_from_edge(0, circ, RELAY_COMMAND_TRUNCATED,
                                     payload, sizeof(payload), NULL);
      }
      return 0;
    case RELAY_COMMAND_TRUNCATED:
      if (!layer_hint) {
        log_fn(LOG_PROTOCOL_WARN, LD_EXIT,
               "'truncated' unsupported at non-origin. Dropping.");
        return 0;
      }
      circuit_truncated(TO_ORIGIN_CIRCUIT(circ), layer_hint,
                        get_uint8(cell->payload + RELAY_HEADER_SIZE));
      return 0;
    case RELAY_COMMAND_CONNECTED:
      if (conn) {
        log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
               "'connected' unsupported while open. Closing circ.");
        return -END_CIRC_REASON_TORPROTOCOL;
      }
      log_info(domain,
               "'connected' received, no conn attached anymore. Ignoring.");
      return 0;
    case RELAY_COMMAND_SENDME:
      if (!rh.stream_id) {
        if (layer_hint) {
          if (layer_hint->package_window + CIRCWINDOW_INCREMENT >
                CIRCWINDOW_START_MAX) {
            log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
                   "Unexpected sendme cell from exit relay. "
                   "Closing circ.");
            return -END_CIRC_REASON_TORPROTOCOL;
          }
          layer_hint->package_window += CIRCWINDOW_INCREMENT;
          log_debug(LD_APP,"circ-level sendme at origin, packagewindow %d.",
                    layer_hint->package_window);
          circuit_resume_edge_reading(circ, layer_hint);
        } else {
          if (circ->package_window + CIRCWINDOW_INCREMENT >
                CIRCWINDOW_START_MAX) {
            log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
                   "Unexpected sendme cell from client. "
                   "Closing circ (window %d).",
                   circ->package_window);
            return -END_CIRC_REASON_TORPROTOCOL;
          }
          circ->package_window += CIRCWINDOW_INCREMENT;
          log_debug(LD_APP,
                    "circ-level sendme at non-origin, packagewindow %d.",
                    circ->package_window);
          circuit_resume_edge_reading(circ, layer_hint);
        }
        return 0;
      }
      if (!conn) {
        log_info(domain,"sendme cell dropped, unknown stream (streamid %d).",
                 rh.stream_id);
        return 0;
      }
      conn->package_window += STREAMWINDOW_INCREMENT;
      log_debug(domain,"stream-level sendme, packagewindow now %d.",
                conn->package_window);
      if (circuit_queue_streams_are_blocked(circ)) {
        /* Still waiting for queue to flush; don't touch conn */
        return 0;
      }
      connection_start_reading(TO_CONN(conn));
      /* handle whatever might still be on the inbuf */
      if (connection_edge_package_raw_inbuf(conn, 1, NULL) < 0) {
        /* (We already sent an end cell if possible) */
        connection_mark_for_close(TO_CONN(conn));
        return 0;
      }
      return 0;
    case RELAY_COMMAND_RESOLVE:
      if (layer_hint) {
        log_fn(LOG_PROTOCOL_WARN, LD_APP,
               "resolve request unsupported at AP; dropping.");
        return 0;
      } else if (conn) {
        log_fn(LOG_PROTOCOL_WARN, domain,
               "resolve request for known stream; dropping.");
        return 0;
      } else if (circ->purpose != CIRCUIT_PURPOSE_OR) {
        log_fn(LOG_PROTOCOL_WARN, domain,
               "resolve request on circ with purpose %d; dropping",
               circ->purpose);
        return 0;
      }
      connection_exit_begin_resolve(cell, TO_OR_CIRCUIT(circ));
      return 0;
    case RELAY_COMMAND_RESOLVED:
      if (conn) {
        log_fn(LOG_PROTOCOL_WARN, domain,
               "'resolved' unsupported while open. Closing circ.");
        return -END_CIRC_REASON_TORPROTOCOL;
      }
      log_info(domain,
               "'resolved' received, no conn attached anymore. Ignoring.");
      return 0;
    case RELAY_COMMAND_ESTABLISH_INTRO:
    case RELAY_COMMAND_ESTABLISH_RENDEZVOUS:
    case RELAY_COMMAND_INTRODUCE1:
    case RELAY_COMMAND_INTRODUCE2:
    case RELAY_COMMAND_INTRODUCE_ACK:
    case RELAY_COMMAND_RENDEZVOUS1:
    case RELAY_COMMAND_RENDEZVOUS2:
    case RELAY_COMMAND_INTRO_ESTABLISHED:
    case RELAY_COMMAND_RENDEZVOUS_ESTABLISHED:
      rend_process_relay_cell(circ, layer_hint,
                              rh.command, rh.length,
                              cell->payload+RELAY_HEADER_SIZE);
      return 0;
  }
  log_fn(LOG_PROTOCOL_WARN, LD_PROTOCOL,
         "Received unknown relay command %d. Perhaps the other side is using "
         "a newer version of Tor? Dropping.",
         rh.command);
  return 0; /* for forward compatibility, don't kill the circuit */
}
