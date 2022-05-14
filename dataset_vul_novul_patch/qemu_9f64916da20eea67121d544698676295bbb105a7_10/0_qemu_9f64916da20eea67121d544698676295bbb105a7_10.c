static void vnc_dpy_update(DisplayState *ds, int x, int y, int w, int h)
{
     int i;
     VncDisplay *vd = ds->opaque;
     struct VncSurface *s = &vd->guest;
    int width = ds_get_width(ds);
    int height = ds_get_height(ds);
 
     h += y;
 
       two 16-pixel blocks but we only mark the first as dirty
    */
    w += (x % 16);
    x -= (x % 16);
     w += (x % 16);
     x -= (x % 16);
 
    x = MIN(x, width);
    y = MIN(y, height);
    w = MIN(x + w, width) - x;
    h = MIN(h, height);
 
     for (; y < h; y++)
         for (i = 0; i < w; i += 16)

void vnc_framebuffer_update(VncState *vs, int x, int y, int w, int h,
                            int32_t encoding)
{
    vnc_write_u16(vs, x);
    vnc_write_u16(vs, y);
    vnc_write_u16(vs, w);
    vnc_write_u16(vs, h);

    vnc_write_s32(vs, encoding);
}

void buffer_reserve(Buffer *buffer, size_t len)
{
    if ((buffer->capacity - buffer->offset) < len) {
        buffer->capacity += (len + 1024);
        buffer->buffer = g_realloc(buffer->buffer, buffer->capacity);
        if (buffer->buffer == NULL) {
            fprintf(stderr, "vnc: out of memory\n");
            exit(1);
        }
    }
}

int buffer_empty(Buffer *buffer)
{
    return buffer->offset == 0;
}

uint8_t *buffer_end(Buffer *buffer)
{
    return buffer->buffer + buffer->offset;
}

void buffer_reset(Buffer *buffer)
{
        buffer->offset = 0;
}

void buffer_free(Buffer *buffer)
{
    g_free(buffer->buffer);
    buffer->offset = 0;
    buffer->capacity = 0;
    buffer->buffer = NULL;
}

void buffer_append(Buffer *buffer, const void *data, size_t len)
{
    memcpy(buffer->buffer + buffer->offset, data, len);
    buffer->offset += len;
}

static void vnc_desktop_resize(VncState *vs)
{
    DisplayState *ds = vs->ds;

    if (vs->csock == -1 || !vnc_has_feature(vs, VNC_FEATURE_RESIZE)) {
        return;
    }
    if (vs->client_width == ds_get_width(ds) &&
        vs->client_height == ds_get_height(ds)) {
        return;
    }
    vs->client_width = ds_get_width(ds);
    vs->client_height = ds_get_height(ds);
    vnc_lock_output(vs);
    vnc_write_u8(vs, VNC_MSG_SERVER_FRAMEBUFFER_UPDATE);
    vnc_write_u8(vs, 0);
    vnc_write_u16(vs, 1); /* number of rects */
    vnc_framebuffer_update(vs, 0, 0, vs->client_width, vs->client_height,
                           VNC_ENCODING_DESKTOPRESIZE);
    vnc_unlock_output(vs);
    vnc_flush(vs);
}

static void vnc_abort_display_jobs(VncDisplay *vd)
{
    VncState *vs;

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_lock_output(vs);
        vs->abort = true;
        vnc_unlock_output(vs);
    }
    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_jobs_join(vs);
    }
    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_lock_output(vs);
        vs->abort = false;
        vnc_unlock_output(vs);
    }
}
     }
