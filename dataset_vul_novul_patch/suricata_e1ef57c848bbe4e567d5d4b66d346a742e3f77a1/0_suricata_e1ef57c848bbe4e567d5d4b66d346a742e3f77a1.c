static void DetectFlow(ThreadVars *tv,
                        DetectEngineCtx *de_ctx, DetectEngineThreadCtx *det_ctx,
                        Packet *p)
 {
    if (p->flags & PKT_NOPACKET_INSPECTION) {
         /* hack: if we are in pass the entire flow mode, we need to still
          * update the inspect_id forward. So test for the condition here,
          * and call the update code if necessary. */
        const int pass = ((p->flow->flags & FLOW_NOPACKET_INSPECTION));
        const AppProto alproto = FlowGetAppProtocol(p->flow);
        if (pass && AppLayerParserProtocolSupportsTxs(p->proto, alproto)) {
            uint8_t flags;
            if (p->flowflags & FLOW_PKT_TOSERVER) {
                flags = STREAM_TOSERVER;
            } else {
                flags = STREAM_TOCLIENT;
            }
             flags = FlowGetDisruptionFlags(p->flow, flags);
             DeStateUpdateInspectTransactionId(p->flow, flags, true);
         }
        SCLogDebug("p->pcap %"PRIu64": no detection on packet, "
                "PKT_NOPACKET_INSPECTION is set", p->pcap_cnt);
         return;
     }
 
    /* see if the packet matches one or more of the sigs */
    (void)DetectRun(tv, de_ctx, det_ctx, p);
}
