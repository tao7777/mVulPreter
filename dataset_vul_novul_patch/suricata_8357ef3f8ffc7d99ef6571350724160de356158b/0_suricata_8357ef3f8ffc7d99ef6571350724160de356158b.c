AppProto AppLayerProtoDetectGetProto(AppLayerProtoDetectThreadCtx *tctx,
                                     Flow *f,
                                     uint8_t *buf, uint32_t buflen,
                                     uint8_t ipproto, uint8_t direction)
{
    SCEnter();
    SCLogDebug("buflen %u for %s direction", buflen,
             (direction & STREAM_TOSERVER) ? "toserver" : "toclient");
 
     AppProto alproto = ALPROTO_UNKNOWN;
    AppProto pm_alproto = ALPROTO_UNKNOWN;
 
     if (!FLOW_IS_PM_DONE(f, direction)) {
         AppProto pm_results[ALPROTO_MAX];
        uint16_t pm_matches = AppLayerProtoDetectPMGetProto(tctx, f,
                                                   buf, buflen,
                                                   direction,
                                                   ipproto,
                                                    pm_results);
         if (pm_matches > 0) {
             alproto = pm_results[0];

            /* HACK: if detected protocol is dcerpc/udp, we run PP as well
             * to avoid misdetecting DNS as DCERPC. */
            if (!(ipproto == IPPROTO_UDP && alproto == ALPROTO_DCERPC))
                goto end;

            pm_alproto = alproto;

            /* fall through */
         }
     }
 
    if (!FLOW_IS_PP_DONE(f, direction)) {
        alproto = AppLayerProtoDetectPPGetProto(f, buf, buflen,
                                                ipproto, direction);
        if (alproto != ALPROTO_UNKNOWN)
            goto end;
    }

    /* Look if flow can be found in expectation list */
    if (!FLOW_IS_PE_DONE(f, direction)) {
        alproto = AppLayerProtoDetectPEGetProto(f, ipproto, direction);
     }
 
  end:
    if (alproto == ALPROTO_UNKNOWN)
        alproto = pm_alproto;

     SCReturnUInt(alproto);
 }
