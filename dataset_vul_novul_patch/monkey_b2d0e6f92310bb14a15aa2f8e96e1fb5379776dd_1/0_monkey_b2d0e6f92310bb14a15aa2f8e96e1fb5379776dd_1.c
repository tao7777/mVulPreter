static inline int mk_vhost_fdt_close(struct session_request *sr)
{
    int id;
    unsigned int hash;
    struct vhost_fdt_hash_table *ht = NULL;
    struct vhost_fdt_hash_chain *hc;

    if (config->fdt == MK_FALSE) {
        return close(sr->fd_file);
    }

    id   = sr->vhost_fdt_id;
    hash = sr->vhost_fdt_hash;

    ht = mk_vhost_fdt_table_lookup(id, sr->host_conf);
    if (mk_unlikely(!ht)) {
        return close(sr->fd_file);
    }

    /* We got the hash table, now look around the chains array */
    hc = mk_vhost_fdt_chain_lookup(hash, ht);
    if (hc) {
        /* Increment the readers and check if we should close */
        hc->readers--;
        if (hc->readers == 0) {
            hc->fd   = -1;
            hc->hash = 0;
            ht->av_slots++;
            return close(sr->fd_file);
        }
        else {
             return 0;
         }
     }
     return close(sr->fd_file);
 }
