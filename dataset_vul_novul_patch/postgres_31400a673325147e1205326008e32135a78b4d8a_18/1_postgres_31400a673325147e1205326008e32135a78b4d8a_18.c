txid_current_snapshot(PG_FUNCTION_ARGS)
{
	TxidSnapshot *snap;
	uint32		nxip,
				i,
				size;
	TxidEpoch	state;
	Snapshot	cur;

	cur = GetActiveSnapshot();
	if (cur == NULL)
		elog(ERROR, "no active snapshot set");
 
 	load_xid_epoch(&state);
 
 	/* allocate */
 	nxip = cur->xcnt;
 	size = TXID_SNAPSHOT_SIZE(nxip);
	snap = palloc(size);
	SET_VARSIZE(snap, size);

	/* fill */
	snap->xmin = convert_xid(cur->xmin, &state);
	snap->xmax = convert_xid(cur->xmax, &state);
	snap->nxip = nxip;
	for (i = 0; i < nxip; i++)
		snap->xip[i] = convert_xid(cur->xip[i], &state);

	/* we want them guaranteed to be in ascending order */
	sort_snapshot(snap);

	PG_RETURN_POINTER(snap);
}
