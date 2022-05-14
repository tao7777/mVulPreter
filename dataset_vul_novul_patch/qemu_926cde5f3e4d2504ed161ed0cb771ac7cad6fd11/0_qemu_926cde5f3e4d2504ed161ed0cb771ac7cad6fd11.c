static void esp_do_dma(ESPState *s)
{
    uint32_t len;
    int to_device;

     len = s->dma_left;
     if (s->do_cmd) {
         trace_esp_do_dma(s->cmdlen, len);
        assert (s->cmdlen <= sizeof(s->cmdbuf) &&
                len <= sizeof(s->cmdbuf) - s->cmdlen);
         s->dma_memory_read(s->dma_opaque, &s->cmdbuf[s->cmdlen], len);
         return;
     }
        return;
    }
