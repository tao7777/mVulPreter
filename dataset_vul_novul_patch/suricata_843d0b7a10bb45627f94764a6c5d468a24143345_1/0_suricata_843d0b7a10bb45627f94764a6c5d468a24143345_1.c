static int StreamTcpPacketStateSynSent(ThreadVars *tv, Packet *p,
                        StreamTcpThread *stt, TcpSession *ssn, PacketQueue *pq)
{
    if (ssn == NULL)
        return -1;

    SCLogDebug("ssn %p: pkt received: %s", ssn, PKT_IS_TOCLIENT(p) ?
               "toclient":"toserver");

    /* RST */
    if (p->tcph->th_flags & TH_RST) {
        if (!StreamTcpValidateRst(ssn, p))
            return -1;

        if (PKT_IS_TOSERVER(p)) {
            if (SEQ_EQ(TCP_GET_SEQ(p), ssn->client.isn) &&
                     SEQ_EQ(TCP_GET_WINDOW(p), 0) &&
                     SEQ_EQ(TCP_GET_ACK(p), (ssn->client.isn + 1)))
             {
                SCLogDebug("ssn->server.flags |= STREAMTCP_STREAM_FLAG_RST_RECV");
                ssn->server.flags |= STREAMTCP_STREAM_FLAG_RST_RECV;

                 StreamTcpPacketSetState(p, ssn, TCP_CLOSED);
                 SCLogDebug("ssn %p: Reset received and state changed to "
                         "TCP_CLOSED", ssn);
             }
         } else {
            ssn->client.flags |= STREAMTCP_STREAM_FLAG_RST_RECV;
            SCLogDebug("ssn->client.flags |= STREAMTCP_STREAM_FLAG_RST_RECV");
             StreamTcpPacketSetState(p, ssn, TCP_CLOSED);
             SCLogDebug("ssn %p: Reset received and state changed to "
                     "TCP_CLOSED", ssn);
        }

    /* FIN */
    } else if (p->tcph->th_flags & TH_FIN) {
        /** \todo */

    /* SYN/ACK */
    } else if ((p->tcph->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
        if ((ssn->flags & STREAMTCP_FLAG_4WHS) && PKT_IS_TOSERVER(p)) {
            SCLogDebug("ssn %p: SYN/ACK received on 4WHS session", ssn);

            /* Check if the SYN/ACK packet ack's the earlier
             * received SYN packet. */
            if (!(SEQ_EQ(TCP_GET_ACK(p), ssn->server.isn + 1))) {
                StreamTcpSetEvent(p, STREAM_4WHS_SYNACK_WITH_WRONG_ACK);

                SCLogDebug("ssn %p: 4WHS ACK mismatch, packet ACK %"PRIu32""
                        " != %" PRIu32 " from stream", ssn,
                        TCP_GET_ACK(p), ssn->server.isn + 1);
                return -1;
            }

            /* Check if the SYN/ACK packet SEQ's the *FIRST* received SYN
             * packet. */
            if (!(SEQ_EQ(TCP_GET_SEQ(p), ssn->client.isn))) {
                StreamTcpSetEvent(p, STREAM_4WHS_SYNACK_WITH_WRONG_SYN);

                SCLogDebug("ssn %p: 4WHS SEQ mismatch, packet SEQ %"PRIu32""
                        " != %" PRIu32 " from *first* SYN pkt", ssn,
                        TCP_GET_SEQ(p), ssn->client.isn);
                return -1;
            }


            /* update state */
            StreamTcpPacketSetState(p, ssn, TCP_SYN_RECV);
            SCLogDebug("ssn %p: =~ 4WHS ssn state is now TCP_SYN_RECV", ssn);

            /* sequence number & window */
            ssn->client.isn = TCP_GET_SEQ(p);
            STREAMTCP_SET_RA_BASE_SEQ(&ssn->client, ssn->client.isn);
            ssn->client.next_seq = ssn->client.isn + 1;

            ssn->server.window = TCP_GET_WINDOW(p);
            SCLogDebug("ssn %p: 4WHS window %" PRIu32 "", ssn,
                    ssn->client.window);

            /* Set the timestamp values used to validate the timestamp of
             * received packets. */
            if ((TCP_HAS_TS(p)) &&
                    (ssn->server.flags & STREAMTCP_STREAM_FLAG_TIMESTAMP))
            {
                ssn->client.last_ts = TCP_GET_TSVAL(p);
                SCLogDebug("ssn %p: 4WHS ssn->client.last_ts %" PRIu32" "
                        "ssn->server.last_ts %" PRIu32"", ssn,
                        ssn->client.last_ts, ssn->server.last_ts);
                ssn->flags |= STREAMTCP_FLAG_TIMESTAMP;
                ssn->client.last_pkt_ts = p->ts.tv_sec;
                if (ssn->client.last_ts == 0)
                    ssn->client.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
            } else {
                ssn->server.last_ts = 0;
                ssn->client.last_ts = 0;
                ssn->server.flags &= ~STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
            }

            ssn->server.last_ack = TCP_GET_ACK(p);
            ssn->client.last_ack = ssn->client.isn + 1;

            /** check for the presense of the ws ptr to determine if we
             *  support wscale at all */
            if ((ssn->flags & STREAMTCP_FLAG_SERVER_WSCALE) &&
                    (TCP_HAS_WSCALE(p)))
            {
                ssn->server.wscale = TCP_GET_WSCALE(p);
            } else {
                ssn->server.wscale = 0;
            }

            if ((ssn->flags & STREAMTCP_FLAG_CLIENT_SACKOK) &&
                    TCP_GET_SACKOK(p) == 1) {
                ssn->flags |= STREAMTCP_FLAG_SACKOK;
                SCLogDebug("ssn %p: SACK permitted for 4WHS session", ssn);
            }

            ssn->client.next_win = ssn->client.last_ack + ssn->client.window;
            ssn->server.next_win = ssn->server.last_ack + ssn->server.window;
            SCLogDebug("ssn %p: 4WHS ssn->client.next_win %" PRIu32 "", ssn,
                    ssn->client.next_win);
            SCLogDebug("ssn %p: 4WHS ssn->server.next_win %" PRIu32 "", ssn,
                    ssn->server.next_win);
            SCLogDebug("ssn %p: 4WHS ssn->client.isn %" PRIu32 ", "
                    "ssn->client.next_seq %" PRIu32 ", "
                    "ssn->client.last_ack %" PRIu32 " "
                    "(ssn->server.last_ack %" PRIu32 ")", ssn,
                    ssn->client.isn, ssn->client.next_seq,
                    ssn->client.last_ack, ssn->server.last_ack);

            /* done here */
            return 0;
        }

        if (PKT_IS_TOSERVER(p)) {
            StreamTcpSetEvent(p, STREAM_3WHS_SYNACK_IN_WRONG_DIRECTION);
            SCLogDebug("ssn %p: SYN/ACK received in the wrong direction", ssn);
            return -1;
        }

        /* Check if the SYN/ACK packet ack's the earlier
         * received SYN packet. */
        if (!(SEQ_EQ(TCP_GET_ACK(p), ssn->client.isn + 1))) {
            StreamTcpSetEvent(p, STREAM_3WHS_SYNACK_WITH_WRONG_ACK);
            SCLogDebug("ssn %p: ACK mismatch, packet ACK %" PRIu32 " != "
                    "%" PRIu32 " from stream", ssn, TCP_GET_ACK(p),
                    ssn->client.isn + 1);
            return -1;
        }

        StreamTcp3whsSynAckUpdate(ssn, p, /* no queue override */NULL);

    } else if (p->tcph->th_flags & TH_SYN) {
        SCLogDebug("ssn %p: SYN packet on state SYN_SENT... resent", ssn);
        if (ssn->flags & STREAMTCP_FLAG_4WHS) {
            SCLogDebug("ssn %p: SYN packet on state SYN_SENT... resent of "
                    "4WHS SYN", ssn);
        }

        if (PKT_IS_TOCLIENT(p)) {
            /** a SYN only packet in the opposite direction could be:
             *  http://www.breakingpointsystems.com/community/blog/tcp-
             *  portals-the-three-way-handshake-is-a-lie
             *
             * \todo improve resetting the session */

            /* indicate that we're dealing with 4WHS here */
            ssn->flags |= STREAMTCP_FLAG_4WHS;
            SCLogDebug("ssn %p: STREAMTCP_FLAG_4WHS flag set", ssn);

            /* set the sequence numbers and window for server
             * We leave the ssn->client.isn in place as we will
             * check the SYN/ACK pkt with that.
             */
            ssn->server.isn = TCP_GET_SEQ(p);
            STREAMTCP_SET_RA_BASE_SEQ(&ssn->server, ssn->server.isn);
            ssn->server.next_seq = ssn->server.isn + 1;

            /* Set the stream timestamp value, if packet has timestamp
             * option enabled. */
            if (TCP_HAS_TS(p)) {
                ssn->server.last_ts = TCP_GET_TSVAL(p);
                SCLogDebug("ssn %p: %02x", ssn, ssn->server.last_ts);

                if (ssn->server.last_ts == 0)
                    ssn->server.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
                ssn->server.last_pkt_ts = p->ts.tv_sec;
                ssn->server.flags |= STREAMTCP_STREAM_FLAG_TIMESTAMP;
            }

            ssn->server.window = TCP_GET_WINDOW(p);
            if (TCP_HAS_WSCALE(p)) {
                ssn->flags |= STREAMTCP_FLAG_SERVER_WSCALE;
                ssn->server.wscale = TCP_GET_WSCALE(p);
            } else {
                ssn->flags &= ~STREAMTCP_FLAG_SERVER_WSCALE;
                ssn->server.wscale = 0;
            }

            if (TCP_GET_SACKOK(p) == 1) {
                ssn->flags |= STREAMTCP_FLAG_CLIENT_SACKOK;
            } else {
                ssn->flags &= ~STREAMTCP_FLAG_CLIENT_SACKOK;
            }

            SCLogDebug("ssn %p: 4WHS ssn->server.isn %" PRIu32 ", "
                    "ssn->server.next_seq %" PRIu32 ", "
                    "ssn->server.last_ack %"PRIu32"", ssn,
                    ssn->server.isn, ssn->server.next_seq,
                    ssn->server.last_ack);
            SCLogDebug("ssn %p: 4WHS ssn->client.isn %" PRIu32 ", "
                    "ssn->client.next_seq %" PRIu32 ", "
                    "ssn->client.last_ack %"PRIu32"", ssn,
                    ssn->client.isn, ssn->client.next_seq,
                    ssn->client.last_ack);
        }

        /** \todo check if it's correct or set event */

    } else if (p->tcph->th_flags & TH_ACK) {
        /* Handle the asynchronous stream, when we receive a  SYN packet
           and now istead of receving a SYN/ACK we receive a ACK from the
           same host, which sent the SYN, this suggests the ASNYC streams.*/
        if (stream_config.async_oneside == FALSE)
            return 0;

        /* we are in AYNC (one side) mode now. */

        /* one side async means we won't see a SYN/ACK, so we can
         * only check the SYN. */
        if (!(SEQ_EQ(TCP_GET_SEQ(p), ssn->client.next_seq))) {
            StreamTcpSetEvent(p, STREAM_3WHS_ASYNC_WRONG_SEQ);

            SCLogDebug("ssn %p: SEQ mismatch, packet SEQ %" PRIu32 " != "
                    "%" PRIu32 " from stream",ssn, TCP_GET_SEQ(p),
                    ssn->client.next_seq);
            return -1;
        }

        ssn->flags |= STREAMTCP_FLAG_ASYNC;
        StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
        SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

        ssn->client.window = TCP_GET_WINDOW(p);
        ssn->client.last_ack = TCP_GET_SEQ(p);
        ssn->client.next_win = ssn->client.last_ack + ssn->client.window;

        /* Set the server side parameters */
        ssn->server.isn = TCP_GET_ACK(p) - 1;
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->server, ssn->server.isn);
        ssn->server.next_seq = ssn->server.isn + 1;
        ssn->server.last_ack = ssn->server.next_seq;
        ssn->server.next_win = ssn->server.last_ack;

        SCLogDebug("ssn %p: synsent => Asynchronous stream, packet SEQ"
                " %" PRIu32 ", payload size %" PRIu32 " (%" PRIu32 "), "
                "ssn->client.next_seq %" PRIu32 ""
                ,ssn, TCP_GET_SEQ(p), p->payload_len, TCP_GET_SEQ(p)
                + p->payload_len, ssn->client.next_seq);

        /* if SYN had wscale, assume it to be supported. Otherwise
         * we know it not to be supported. */
        if (ssn->flags & STREAMTCP_FLAG_SERVER_WSCALE) {
            ssn->client.wscale = TCP_WSCALE_MAX;
        }

        /* Set the timestamp values used to validate the timestamp of
         * received packets.*/
        if (TCP_HAS_TS(p) &&
                (ssn->client.flags & STREAMTCP_STREAM_FLAG_TIMESTAMP))
        {
            ssn->flags |= STREAMTCP_FLAG_TIMESTAMP;
            ssn->client.flags &= ~STREAMTCP_STREAM_FLAG_TIMESTAMP;
            ssn->client.last_pkt_ts = p->ts.tv_sec;
        } else {
            ssn->client.last_ts = 0;
            ssn->client.flags &= ~STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
        }

        if (ssn->flags & STREAMTCP_FLAG_CLIENT_SACKOK) {
            ssn->flags |= STREAMTCP_FLAG_SACKOK;
        }

        StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                &ssn->client, p, pq);

    } else {
        SCLogDebug("ssn %p: default case", ssn);
    }

    return 0;
}
