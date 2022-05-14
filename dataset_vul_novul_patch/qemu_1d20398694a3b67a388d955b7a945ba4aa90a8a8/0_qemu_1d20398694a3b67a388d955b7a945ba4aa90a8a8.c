static void coroutine_fn v9fs_wstat(void *opaque)
{
    int32_t fid;
    int err = 0;
    int16_t unused;
    V9fsStat v9stat;
    size_t offset = 7;
     struct stat stbuf;
     V9fsFidState *fidp;
     V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;
 
     v9fs_stat_init(&v9stat);
     err = pdu_unmarshal(pdu, offset, "dwS", &fid, &unused, &v9stat);
        goto out_nofid;
    }
