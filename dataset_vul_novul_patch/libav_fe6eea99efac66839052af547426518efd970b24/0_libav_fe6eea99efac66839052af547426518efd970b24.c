static int nsv_read_chunk(AVFormatContext *s, int fill_header)
{
    NSVContext *nsv = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st[2] = {NULL, NULL};
    NSVStream *nst;
    AVPacket *pkt;
    int i, err = 0;
    uint8_t auxcount; /* number of aux metadata, also 4 bits of vsize */
     uint32_t vsize;
     uint16_t asize;
     uint16_t auxsize;
    int ret;
 
     if (nsv->ahead[0].data || nsv->ahead[1].data)
         return 0; //-1; /* hey! eat what you've in your plate first! */

null_chunk_retry:
    if (pb->eof_reached)
        return -1;

    for (i = 0; i < NSV_MAX_RESYNC_TRIES && nsv->state < NSV_FOUND_NSVS && !err; i++)
        err = nsv_resync(s);
    if (err < 0)
        return err;
    if (nsv->state == NSV_FOUND_NSVS)
        err = nsv_parse_NSVs_header(s);
    if (err < 0)
        return err;
    if (nsv->state != NSV_HAS_READ_NSVS && nsv->state != NSV_FOUND_BEEF)
        return -1;

    auxcount = avio_r8(pb);
    vsize = avio_rl16(pb);
    asize = avio_rl16(pb);
    vsize = (vsize << 4) | (auxcount >> 4);
    auxcount &= 0x0f;
    av_log(s, AV_LOG_TRACE, "NSV CHUNK %"PRIu8" aux, %"PRIu32" bytes video, %"PRIu16" bytes audio\n",
           auxcount, vsize, asize);
    /* skip aux stuff */
    for (i = 0; i < auxcount; i++) {
        uint32_t av_unused auxtag;
        auxsize = avio_rl16(pb);
        auxtag = avio_rl32(pb);
        avio_skip(pb, auxsize);
        vsize -= auxsize + sizeof(uint16_t) + sizeof(uint32_t); /* that's becoming brain-dead */
    }

    if (pb->eof_reached)
        return -1;
    if (!vsize && !asize) {
        nsv->state = NSV_UNSYNC;
        goto null_chunk_retry;
    }

    /* map back streams to v,a */
    if (s->nb_streams > 0)
        st[s->streams[0]->id] = s->streams[0];
    if (s->nb_streams > 1)
        st[s->streams[1]->id] = s->streams[1];

     if (vsize && st[NSV_ST_VIDEO]) {
         nst = st[NSV_ST_VIDEO]->priv_data;
         pkt = &nsv->ahead[NSV_ST_VIDEO];
        if ((ret = av_get_packet(pb, pkt, vsize)) < 0)
            return ret;
         pkt->stream_index = st[NSV_ST_VIDEO]->index;//NSV_ST_VIDEO;
         pkt->dts = nst->frame_offset;
         pkt->flags |= nsv->state == NSV_HAS_READ_NSVS ? AV_PKT_FLAG_KEY : 0; /* keyframe only likely on a sync frame */
        for (i = 0; i < FFMIN(8, vsize); i++)
            av_log(s, AV_LOG_TRACE, "NSV video: [%d] = %02"PRIx8"\n",
                   i, pkt->data[i]);
    }
    if(st[NSV_ST_VIDEO])
        ((NSVStream*)st[NSV_ST_VIDEO]->priv_data)->frame_offset++;

    if (asize && st[NSV_ST_AUDIO]) {
        nst = st[NSV_ST_AUDIO]->priv_data;
        pkt = &nsv->ahead[NSV_ST_AUDIO];
        /* read raw audio specific header on the first audio chunk... */
        /* on ALL audio chunks ?? seems so! */
        if (asize && st[NSV_ST_AUDIO]->codecpar->codec_tag == MKTAG('P', 'C', 'M', ' ')/* && fill_header*/) {
            uint8_t bps;
            uint8_t channels;
            uint16_t samplerate;
            bps = avio_r8(pb);
            channels = avio_r8(pb);
            samplerate = avio_rl16(pb);
            if (!channels || !samplerate)
                return AVERROR_INVALIDDATA;
            asize-=4;
            av_log(s, AV_LOG_TRACE, "NSV RAWAUDIO: bps %"PRIu8", nchan %"PRIu8", srate %"PRIu16"\n",
                   bps, channels, samplerate);
            if (fill_header) {
                st[NSV_ST_AUDIO]->need_parsing = AVSTREAM_PARSE_NONE; /* we know everything */
                if (bps != 16) {
                    av_log(s, AV_LOG_TRACE, "NSV AUDIO bit/sample != 16 (%"PRIu8")!!!\n", bps);
                }
                bps /= channels; // ???
                if (bps == 8)
                    st[NSV_ST_AUDIO]->codecpar->codec_id = AV_CODEC_ID_PCM_U8;
                samplerate /= 4;/* UGH ??? XXX */
                channels = 1;
                st[NSV_ST_AUDIO]->codecpar->channels = channels;
                st[NSV_ST_AUDIO]->codecpar->sample_rate = samplerate;
                av_log(s, AV_LOG_TRACE, "NSV RAWAUDIO: bps %"PRIu8", nchan %"PRIu8", srate %"PRIu16"\n",
                        bps, channels, samplerate);
             }
         }
        if ((ret = av_get_packet(pb, pkt, asize)) < 0)
            return ret;
         pkt->stream_index = st[NSV_ST_AUDIO]->index;//NSV_ST_AUDIO;
         pkt->flags |= nsv->state == NSV_HAS_READ_NSVS ? AV_PKT_FLAG_KEY : 0; /* keyframe only likely on a sync frame */
         if( nsv->state == NSV_HAS_READ_NSVS && st[NSV_ST_VIDEO] ) {
            /* on a nsvs frame we have new information on a/v sync */
            pkt->dts = (((NSVStream*)st[NSV_ST_VIDEO]->priv_data)->frame_offset-1);
            pkt->dts *= (int64_t)1000        * nsv->framerate.den;
            pkt->dts += (int64_t)nsv->avsync * nsv->framerate.num;
            av_log(s, AV_LOG_TRACE, "NSV AUDIO: sync:%"PRId16", dts:%"PRId64,
                   nsv->avsync, pkt->dts);
        }
        nst->frame_offset++;
    }

    nsv->state = NSV_UNSYNC;
    return 0;
}
