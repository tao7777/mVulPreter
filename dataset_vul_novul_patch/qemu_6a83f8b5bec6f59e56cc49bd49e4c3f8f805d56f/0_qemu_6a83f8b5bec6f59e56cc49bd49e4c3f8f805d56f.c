int qcow2_snapshot_load_tmp(BlockDriverState *bs,
                            const char *snapshot_id,
                            const char *name,
                            Error **errp)
{
    int i, snapshot_index;
    BDRVQcowState *s = bs->opaque;
    QCowSnapshot *sn;
    uint64_t *new_l1_table;
    int new_l1_bytes;
    int ret;

    assert(bs->read_only);

    /* Search the snapshot */
    snapshot_index = find_snapshot_by_id_and_name(bs, snapshot_id, name);
    if (snapshot_index < 0) {
        error_setg(errp,
                   "Can't find snapshot");
        return -ENOENT;
    }
     sn = &s->snapshots[snapshot_index];
 
     /* Allocate and read in the snapshot's L1 table */
    if (sn->l1_size > QCOW_MAX_L1_SIZE) {
        error_setg(errp, "Snapshot L1 table too large");
        return -EFBIG;
    }
     new_l1_bytes = sn->l1_size * sizeof(uint64_t);
     new_l1_table = g_malloc0(align_offset(new_l1_bytes, 512));
        return ret;
    }
