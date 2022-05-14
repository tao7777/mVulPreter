static int nbd_negotiate_read(QIOChannel *ioc, void *buffer, size_t size)
{
    ssize_t ret;
    guint watch;
    assert(qemu_in_coroutine());
    /* Negotiation are always in main loop. */
    watch = qio_channel_add_watch(ioc,
                                  G_IO_IN,
                                  nbd_negotiate_continue,
                                  qemu_coroutine_self(),
                                  NULL);
    ret = nbd_read(ioc, buffer, size, NULL);
    g_source_remove(watch);
    return ret;
}
