static void DetectRunCleanup(DetectEngineThreadCtx *det_ctx,
        Packet *p, Flow * const pflow)
{
    PACKET_PROFILING_DETECT_START(p, PROF_DETECT_CLEANUP);
    /* cleanup pkt specific part of the patternmatcher */
    PacketPatternCleanup(det_ctx);
 
     if (pflow != NULL) {
         /* update inspected tracker for raw reassembly */
        if (p->proto == IPPROTO_TCP && pflow->protoctx != NULL &&
            (p->flags & PKT_STREAM_EST))
        {
             StreamReassembleRawUpdateProgress(pflow->protoctx, p,
                     det_ctx->raw_stream_progress);
 
            DetectEngineCleanHCBDBuffers(det_ctx);
        }
    }
    PACKET_PROFILING_DETECT_END(p, PROF_DETECT_CLEANUP);
    SCReturn;
}
