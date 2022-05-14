static int virtio_net_load(QEMUFile *f, void *opaque, int version_id)
{
    VirtIONet *n = opaque;
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    int ret, i, link_down;

    if (version_id < 2 || version_id > VIRTIO_NET_VM_VERSION)
        return -EINVAL;

    ret = virtio_load(vdev, f);
    if (ret) {
        return ret;
    }

    qemu_get_buffer(f, n->mac, ETH_ALEN);
    n->vqs[0].tx_waiting = qemu_get_be32(f);

    virtio_net_set_mrg_rx_bufs(n, qemu_get_be32(f));

    if (version_id >= 3)
        n->status = qemu_get_be16(f);

    if (version_id >= 4) {
        if (version_id < 8) {
            n->promisc = qemu_get_be32(f);
            n->allmulti = qemu_get_be32(f);
        } else {
            n->promisc = qemu_get_byte(f);
            n->allmulti = qemu_get_byte(f);
        }
    }

    if (version_id >= 5) {
        n->mac_table.in_use = qemu_get_be32(f);
        /* MAC_TABLE_ENTRIES may be different from the saved image */
         if (n->mac_table.in_use <= MAC_TABLE_ENTRIES) {
             qemu_get_buffer(f, n->mac_table.macs,
                             n->mac_table.in_use * ETH_ALEN);
        } else {
            int64_t i;

            /* Overflow detected - can happen if source has a larger MAC table.
             * We simply set overflow flag so there's no need to maintain the
             * table of addresses, discard them all.
             * Note: 64 bit math to avoid integer overflow.
             */
            for (i = 0; i < (int64_t)n->mac_table.in_use * ETH_ALEN; ++i) {
                qemu_get_byte(f);
            }
             n->mac_table.multi_overflow = n->mac_table.uni_overflow = 1;
             n->mac_table.in_use = 0;
         }
            error_report("virtio-net: saved image requires vnet_hdr=on");
            return -1;
        }
    }
