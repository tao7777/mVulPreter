static int mpeg4video_probe(AVProbeData *probe_packet)
{
    uint32_t temp_buffer = -1;
    int VO = 0, VOL = 0, VOP = 0, VISO = 0, res = 0;
    int i;
 
     for (i = 0; i < probe_packet->buf_size; i++) {
         temp_buffer = (temp_buffer << 8) + probe_packet->buf[i];
        if (temp_buffer & 0xfffffe00)
            continue;
        if (temp_buffer < 2)
             continue;
 
         if (temp_buffer == VOP_START_CODE)
             VOP++;
         else if (temp_buffer == VISUAL_OBJECT_START_CODE)
             VISO++;
        else if (temp_buffer >= 0x100 && temp_buffer < 0x120)
             VO++;
        else if (temp_buffer >= 0x120 && temp_buffer < 0x130)
             VOL++;
         else if (!(0x1AF < temp_buffer && temp_buffer < 0x1B7) &&
                  !(0x1B9 < temp_buffer && temp_buffer < 0x1C4))
            res++;
    }

    if (VOP >= VISO && VOP >= VOL && VO >= VOL && VOL > 0 && res == 0)
        return AVPROBE_SCORE_EXTENSION;
    return 0;
}
