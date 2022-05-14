static int nbd_negotiate_drop_sync(QIOChannel *ioc, size_t size)
{
    ssize_t ret;
    uint8_t *buffer = g_malloc(MIN(65536, size));
    while (size > 0) {
        size_t count = MIN(65536, size);
        ret = nbd_negotiate_read(ioc, buffer, count);
        if (ret < 0) {
            g_free(buffer);
            return ret;
        }
        size -= count;
    }
    g_free(buffer);
    return 0;
}
