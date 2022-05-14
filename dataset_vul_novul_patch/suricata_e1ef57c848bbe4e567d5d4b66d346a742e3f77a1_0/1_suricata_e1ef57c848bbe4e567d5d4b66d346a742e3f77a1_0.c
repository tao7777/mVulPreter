int StreamTcpPacket (ThreadVars *tv, Packet *p, StreamTcpThread *stt,
                     PacketQueue *pq)
{
    SCEnter();

    DEBUG_ASSERT_FLOW_LOCKED(p->flow);

    SCLogDebug("p->pcap_cnt %"PRIu64, p->pcap_cnt);

    /* assign the thread id to the flow */
    if (unlikely(p->flow->thread_id == 0)) {
        p->flow->thread_id = (FlowThreadId)tv->id;
#ifdef DEBUG
    } else if (unlikely((FlowThreadId)tv->id != p->flow->thread_id)) {
        SCLogDebug("wrong thread: flow has %u, we are %d", p->flow->thread_id, tv->id);
#endif
    }

    TcpSession *ssn = (TcpSession *)p->flow->protoctx;

    /* track TCP flags */
    if (ssn != NULL) {
        ssn->tcp_packet_flags |= p->tcph->th_flags;
        if (PKT_IS_TOSERVER(p))
            ssn->client.tcp_flags |= p->tcph->th_flags;
        else if (PKT_IS_TOCLIENT(p))
            ssn->server.tcp_flags |= p->tcph->th_flags;

        /* check if we need to unset the ASYNC flag */
        if (ssn->flags & STREAMTCP_FLAG_ASYNC &&
            ssn->client.tcp_flags != 0 &&
            ssn->server.tcp_flags != 0)
        {
            SCLogDebug("ssn %p: removing ASYNC flag as we have packets on both sides", ssn);
            ssn->flags &= ~STREAMTCP_FLAG_ASYNC;
        }
    }

    /* update counters */
    if ((p->tcph->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
        StatsIncr(tv, stt->counter_tcp_synack);
    } else if (p->tcph->th_flags & (TH_SYN)) {
        StatsIncr(tv, stt->counter_tcp_syn);
    }
    if (p->tcph->th_flags & (TH_RST)) {
        StatsIncr(tv, stt->counter_tcp_rst);
    }

    /* broken TCP http://ask.wireshark.org/questions/3183/acknowledgment-number-broken-tcp-the-acknowledge-field-is-nonzero-while-the-ack-flag-is-not-set */
    if (!(p->tcph->th_flags & TH_ACK) && TCP_GET_ACK(p) != 0) {
        StreamTcpSetEvent(p, STREAM_PKT_BROKEN_ACK);
    }

    /* If we are on IPS mode, and got a drop action triggered from
     * the IP only module, or from a reassembled msg and/or from an
     * applayer detection, then drop the rest of the packets of the
     * same stream and avoid inspecting it any further */
    if (StreamTcpCheckFlowDrops(p) == 1) {
        SCLogDebug("This flow/stream triggered a drop rule");
        FlowSetNoPacketInspectionFlag(p->flow);
        DecodeSetNoPacketInspectionFlag(p);
        StreamTcpDisableAppLayer(p->flow);
        PACKET_DROP(p);
        /* return the segments to the pool */
        StreamTcpSessionPktFree(p);
        SCReturnInt(0);
    }

    if (ssn == NULL || ssn->state == TCP_NONE) {
        if (StreamTcpPacketStateNone(tv, p, stt, ssn, &stt->pseudo_queue) == -1) {
            goto error;
        }

        if (ssn != NULL)
            SCLogDebug("ssn->alproto %"PRIu16"", p->flow->alproto);
    } else {
        /* special case for PKT_PSEUDO_STREAM_END packets:
         * bypass the state handling and various packet checks,
         * we care about reassembly here. */
        if (p->flags & PKT_PSEUDO_STREAM_END) {
            if (PKT_IS_TOCLIENT(p)) {
                ssn->client.last_ack = TCP_GET_ACK(p);
                StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                        &ssn->server, p, pq);
            } else {
                ssn->server.last_ack = TCP_GET_ACK(p);
                StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                        &ssn->client, p, pq);
            }
            /* straight to 'skip' as we already handled reassembly */
            goto skip;
        }

        /* check if the packet is in right direction, when we missed the
           SYN packet and picked up midstream session. */
        if (ssn->flags & STREAMTCP_FLAG_MIDSTREAM_SYNACK)
            StreamTcpPacketSwitchDir(ssn, p);

        if (StreamTcpPacketIsKeepAlive(ssn, p) == 1) {
            goto skip;
        }
        if (StreamTcpPacketIsKeepAliveACK(ssn, p) == 1) {
            StreamTcpClearKeepAliveFlag(ssn, p);
            goto skip;
        }
        StreamTcpClearKeepAliveFlag(ssn, p);

        /* if packet is not a valid window update, check if it is perhaps
         * a bad window update that we should ignore (and alert on) */
        if (StreamTcpPacketIsFinShutdownAck(ssn, p) == 0)
            if (StreamTcpPacketIsWindowUpdate(ssn, p) == 0)
                if (StreamTcpPacketIsBadWindowUpdate(ssn,p))
                    goto skip;

        switch (ssn->state) {
            case TCP_SYN_SENT:
                if(StreamTcpPacketStateSynSent(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_SYN_RECV:
                if(StreamTcpPacketStateSynRecv(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_ESTABLISHED:
                if(StreamTcpPacketStateEstablished(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_FIN_WAIT1:
                if(StreamTcpPacketStateFinWait1(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_FIN_WAIT2:
                if(StreamTcpPacketStateFinWait2(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_CLOSING:
                if(StreamTcpPacketStateClosing(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_CLOSE_WAIT:
                if(StreamTcpPacketStateCloseWait(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_LAST_ACK:
                if(StreamTcpPacketStateLastAck(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_TIME_WAIT:
                if(StreamTcpPacketStateTimeWait(tv, p, stt, ssn, &stt->pseudo_queue)) {
                    goto error;
                }
                break;
            case TCP_CLOSED:
                /* TCP session memory is not returned to pool until timeout. */
                SCLogDebug("packet received on closed state");
                break;
            default:
                SCLogDebug("packet received on default state");
                break;
        }
    skip:

        if (ssn->state >= TCP_ESTABLISHED) {
            p->flags |= PKT_STREAM_EST;
        }
    }

    /* deal with a pseudo packet that is created upon receiving a RST
     * segment. To be sure we process both sides of the connection, we
     * inject a fake packet into the system, forcing reassembly of the
     * opposing direction.
     * There should be only one, but to be sure we do a while loop. */
    if (ssn != NULL) {
        while (stt->pseudo_queue.len > 0) {
            SCLogDebug("processing pseudo packet / stream end");
            Packet *np = PacketDequeue(&stt->pseudo_queue);
            if (np != NULL) {
                /* process the opposing direction of the original packet */
                if (PKT_IS_TOSERVER(np)) {
                    SCLogDebug("pseudo packet is to server");
                    StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                            &ssn->client, np, NULL);
                } else {
                    SCLogDebug("pseudo packet is to client");
                    StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                            &ssn->server, np, NULL);
                }

                /* enqueue this packet so we inspect it in detect etc */
                PacketEnqueue(pq, np);
            }
            SCLogDebug("processing pseudo packet / stream end done");
        }

        /* recalc the csum on the packet if it was modified */
        if (p->flags & PKT_STREAM_MODIFIED) {
            ReCalculateChecksum(p);
        }

        /* check for conditions that may make us not want to log this packet */

        /* streams that hit depth */
        if ((ssn->client.flags & STREAMTCP_STREAM_FLAG_DEPTH_REACHED) &&
             (ssn->server.flags & STREAMTCP_STREAM_FLAG_DEPTH_REACHED))
        {
            /* we can call bypass callback, if enabled */
            if (StreamTcpBypassEnabled()) {
                PacketBypassCallback(p);
            }
        }

        if ((ssn->client.flags & STREAMTCP_STREAM_FLAG_DEPTH_REACHED) ||
             (ssn->server.flags & STREAMTCP_STREAM_FLAG_DEPTH_REACHED))
        {
            p->flags |= PKT_STREAM_NOPCAPLOG;
        }

        /* encrypted packets */
        if ((PKT_IS_TOSERVER(p) && (ssn->client.flags & STREAMTCP_STREAM_FLAG_NOREASSEMBLY)) ||
            (PKT_IS_TOCLIENT(p) && (ssn->server.flags & STREAMTCP_STREAM_FLAG_NOREASSEMBLY)))
        {
            p->flags |= PKT_STREAM_NOPCAPLOG;
        }

        if (ssn->flags & STREAMTCP_FLAG_BYPASS) {
            /* we can call bypass callback, if enabled */
            if (StreamTcpBypassEnabled()) {
                PacketBypassCallback(p);
            }

        /* if stream is dead and we have no detect engine at all, bypass. */
        } else if (g_detect_disabled &&
                (ssn->client.flags & STREAMTCP_STREAM_FLAG_NOREASSEMBLY) &&
                (ssn->server.flags & STREAMTCP_STREAM_FLAG_NOREASSEMBLY) &&
                StreamTcpBypassEnabled())
        {
            SCLogDebug("bypass as stream is dead and we have no rules");
            PacketBypassCallback(p);
        }
    }

    SCReturnInt(0);

error:
    /* make sure we don't leave packets in our pseudo queue */
    while (stt->pseudo_queue.len > 0) {
        Packet *np = PacketDequeue(&stt->pseudo_queue);
        if (np != NULL) {
            PacketEnqueue(pq, np);
        }
    }

    /* recalc the csum on the packet if it was modified */
    if (p->flags & PKT_STREAM_MODIFIED) {
        ReCalculateChecksum(p);
     }
 
     if (StreamTcpInlineDropInvalid()) {
         PACKET_DROP(p);
     }
     SCReturnInt(-1);
}
