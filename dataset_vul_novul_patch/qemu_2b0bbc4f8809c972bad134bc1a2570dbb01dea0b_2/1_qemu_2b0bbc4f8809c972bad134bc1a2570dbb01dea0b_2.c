static int nbd_negotiate_write(QIOChannel *ioc, const void *buffer, size_t size)
{
    ssize_t ret;
    guint watch;
    assert(qemu_in_coroutine());
    /* Negotiation are always in main loop. */
    watch = qio_channel_add_watch(ioc,
                                  G_IO_OUT,
                                  nbd_negotiate_continue,
                                  qemu_coroutine_self(),
                                  NULL);
    ret = nbd_write(ioc, buffer, size, NULL);
    g_source_remove(watch);
    return ret;
}
