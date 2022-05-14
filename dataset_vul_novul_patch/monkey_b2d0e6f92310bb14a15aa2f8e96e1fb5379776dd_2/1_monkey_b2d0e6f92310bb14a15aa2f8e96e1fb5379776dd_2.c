static inline int mk_vhost_fdt_open(int id, unsigned int hash,
                                    struct session_request *sr)
{
    int i;
    int fd;
    struct vhost_fdt_hash_table *ht = NULL;
    struct vhost_fdt_hash_chain *hc;

    if (config->fdt == MK_FALSE) {
        return open(sr->real_path.data, sr->file_info.flags_read_only);
    }

    ht = mk_vhost_fdt_table_lookup(id, sr->host_conf);
    if (mk_unlikely(!ht)) {
        return open(sr->real_path.data, sr->file_info.flags_read_only);
    }

    /* We got the hash table, now look around the chains array */
    hc = mk_vhost_fdt_chain_lookup(hash, ht);
    if (hc) {
        /* Increment the readers and return the shared FD */
        hc->readers++;
        return hc->fd;
    }

    /*
     * Get here means that no entry exists in the hash table for the
     * requested file descriptor and hash, we must try to open the file
     * and register the entry in the table.
     */
    fd = open(sr->real_path.data, sr->file_info.flags_read_only);
    if (fd == -1) {
        return -1;
    }

    /* If chains are full, just return the new FD, bad luck... */
    if (ht->av_slots <= 0) {
        return fd;
    }

    /* Register the new entry in an available slot */
    for (i = 0; i < VHOST_FDT_HASHTABLE_CHAINS; i++) {
        hc = &ht->chain[i];
        if (hc->fd == -1) {
            hc->fd   = fd;
            hc->hash = hash;
            hc->readers++;
            ht->av_slots--;
 
             sr->vhost_fdt_id   = id;
             sr->vhost_fdt_hash = hash;
 
             return fd;
         }
    }

    return -1;
}
