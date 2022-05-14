 static void v9fs_walk(void *opaque)
 {
     int name_idx;
    V9fsFidState *newfidp = NULL;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    err = pdu_unmarshal(pdu, offset, "ddw", &fid, &newfid, &nwnames);
    if (err < 0) {
        pdu_complete(pdu, err);
        return ;
    }
     V9fsFidState *newfidp = NULL;
     V9fsPDU *pdu = opaque;
     V9fsState *s = pdu->s;
    V9fsQID qid;
 
     err = pdu_unmarshal(pdu, offset, "ddw", &fid, &newfid, &nwnames);
     if (err < 0) {
        for (i = 0; i < nwnames; i++) {
            err = pdu_unmarshal(pdu, offset, "s", &wnames[i]);
            if (err < 0) {
                goto out_nofid;
            }
            if (name_is_illegal(wnames[i].data)) {
                err = -ENOENT;
                goto out_nofid;
            }
            offset += err;
        }
    } else if (nwnames > P9_MAXWELEM) {
        err = -EINVAL;
        goto out_nofid;
    }
    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -ENOENT;
        goto out_nofid;
    }
    v9fs_path_init(&dpath);
    v9fs_path_init(&path);
    /*
     * Both dpath and path initially poin to fidp.
     * Needed to handle request with nwnames == 0
     */
    v9fs_path_copy(&dpath, &fidp->path);
         err = -ENOENT;
         goto out_nofid;
     }
