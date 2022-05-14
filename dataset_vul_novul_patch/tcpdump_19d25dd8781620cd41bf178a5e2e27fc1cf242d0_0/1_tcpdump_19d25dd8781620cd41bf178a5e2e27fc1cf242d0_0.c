nfsreq_print_noaddr(netdissect_options *ndo,
                    register const u_char *bp, u_int length,
                    register const u_char *bp2)
{
	register const struct sunrpc_msg *rp;
	register const uint32_t *dp;
	nfs_type type;
	int v3;
	uint32_t proc;
	uint32_t access_flags;
	struct nfsv3_sattr sa3;

	ND_PRINT((ndo, "%d", length));
	nfserr = 0;		/* assume no error */
	rp = (const struct sunrpc_msg *)bp;

	if (!xid_map_enter(ndo, rp, bp2))	/* record proc number for later on */
		goto trunc;

	v3 = (EXTRACT_32BITS(&rp->rm_call.cb_vers) == NFS_VER3);
	proc = EXTRACT_32BITS(&rp->rm_call.cb_proc);

	if (!v3 && proc < NFS_NPROCS)
		proc =  nfsv3_procid[proc];

	ND_PRINT((ndo, " %s", tok2str(nfsproc_str, "proc-%u", proc)));
	switch (proc) {

	case NFSPROC_GETATTR:
	case NFSPROC_SETATTR:
	case NFSPROC_READLINK:
	case NFSPROC_FSSTAT:
	case NFSPROC_FSINFO:
	case NFSPROC_PATHCONF:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    parsefh(ndo, dp, v3) != NULL)
			return;
		break;

	case NFSPROC_LOOKUP:
	case NFSPROC_CREATE:
	case NFSPROC_MKDIR:
	case NFSPROC_REMOVE:
	case NFSPROC_RMDIR:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    parsefhn(ndo, dp, v3) != NULL)
			return;
		break;

	case NFSPROC_ACCESS:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			ND_TCHECK(dp[0]);
			access_flags = EXTRACT_32BITS(&dp[0]);
			if (access_flags & ~NFSV3ACCESS_FULL) {
				/* NFSV3ACCESS definitions aren't up to date */
				ND_PRINT((ndo, " %04x", access_flags));
			} else if ((access_flags & NFSV3ACCESS_FULL) == NFSV3ACCESS_FULL) {
				ND_PRINT((ndo, " NFS_ACCESS_FULL"));
			} else {
				char separator = ' ';
				if (access_flags & NFSV3ACCESS_READ) {
					ND_PRINT((ndo, " NFS_ACCESS_READ"));
					separator = '|';
				}
				if (access_flags & NFSV3ACCESS_LOOKUP) {
					ND_PRINT((ndo, "%cNFS_ACCESS_LOOKUP", separator));
					separator = '|';
				}
				if (access_flags & NFSV3ACCESS_MODIFY) {
					ND_PRINT((ndo, "%cNFS_ACCESS_MODIFY", separator));
					separator = '|';
				}
				if (access_flags & NFSV3ACCESS_EXTEND) {
					ND_PRINT((ndo, "%cNFS_ACCESS_EXTEND", separator));
					separator = '|';
				}
				if (access_flags & NFSV3ACCESS_DELETE) {
					ND_PRINT((ndo, "%cNFS_ACCESS_DELETE", separator));
					separator = '|';
				}
				if (access_flags & NFSV3ACCESS_EXECUTE)
					ND_PRINT((ndo, "%cNFS_ACCESS_EXECUTE", separator));
			}
			return;
		}
		break;

	case NFSPROC_READ:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			if (v3) {
				ND_TCHECK(dp[2]);
				ND_PRINT((ndo, " %u bytes @ %" PRIu64,
				       EXTRACT_32BITS(&dp[2]),
				       EXTRACT_64BITS(&dp[0])));
			} else {
				ND_TCHECK(dp[1]);
				ND_PRINT((ndo, " %u bytes @ %u",
				    EXTRACT_32BITS(&dp[1]),
				    EXTRACT_32BITS(&dp[0])));
			}
			return;
		}
		break;

	case NFSPROC_WRITE:
 		if ((dp = parsereq(ndo, rp, length)) != NULL &&
 		    (dp = parsefh(ndo, dp, v3)) != NULL) {
 			if (v3) {
				ND_TCHECK(dp[2]);
 				ND_PRINT((ndo, " %u (%u) bytes @ %" PRIu64,
 						EXTRACT_32BITS(&dp[4]),
 						EXTRACT_32BITS(&dp[2]),
 						EXTRACT_64BITS(&dp[0])));
 				if (ndo->ndo_vflag) {
					dp += 3;
					ND_TCHECK(dp[0]);
 					ND_PRINT((ndo, " <%s>",
 						tok2str(nfsv3_writemodes,
							NULL, EXTRACT_32BITS(dp))));
 				}
 			} else {
 				ND_TCHECK(dp[3]);
				ND_PRINT((ndo, " %u (%u) bytes @ %u (%u)",
						EXTRACT_32BITS(&dp[3]),
						EXTRACT_32BITS(&dp[2]),
						EXTRACT_32BITS(&dp[1]),
						EXTRACT_32BITS(&dp[0])));
			}
			return;
		}
		break;

	case NFSPROC_SYMLINK:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefhn(ndo, dp, v3)) != NULL) {
			ND_PRINT((ndo, " ->"));
			if (v3 && (dp = parse_sattr3(ndo, dp, &sa3)) == NULL)
				break;
			if (parsefn(ndo, dp) == NULL)
				break;
			if (v3 && ndo->ndo_vflag)
				print_sattr3(ndo, &sa3, ndo->ndo_vflag);
			return;
		}
		break;

	case NFSPROC_MKNOD:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefhn(ndo, dp, v3)) != NULL) {
			ND_TCHECK(*dp);
			type = (nfs_type)EXTRACT_32BITS(dp);
			dp++;
			if ((dp = parse_sattr3(ndo, dp, &sa3)) == NULL)
				break;
			ND_PRINT((ndo, " %s", tok2str(type2str, "unk-ft %d", type)));
			if (ndo->ndo_vflag && (type == NFCHR || type == NFBLK)) {
				ND_TCHECK(dp[1]);
				ND_PRINT((ndo, " %u/%u",
				       EXTRACT_32BITS(&dp[0]),
				       EXTRACT_32BITS(&dp[1])));
				dp += 2;
			}
			if (ndo->ndo_vflag)
				print_sattr3(ndo, &sa3, ndo->ndo_vflag);
			return;
		}
		break;

	case NFSPROC_RENAME:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefhn(ndo, dp, v3)) != NULL) {
			ND_PRINT((ndo, " ->"));
			if (parsefhn(ndo, dp, v3) != NULL)
				return;
		}
		break;

	case NFSPROC_LINK:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			ND_PRINT((ndo, " ->"));
			if (parsefhn(ndo, dp, v3) != NULL)
				return;
		}
		break;

	case NFSPROC_READDIR:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			if (v3) {
				ND_TCHECK(dp[4]);
				/*
				 * We shouldn't really try to interpret the
				 * offset cookie here.
				 */
				ND_PRINT((ndo, " %u bytes @ %" PRId64,
				    EXTRACT_32BITS(&dp[4]),
				    EXTRACT_64BITS(&dp[0])));
				if (ndo->ndo_vflag)
					ND_PRINT((ndo, " verf %08x%08x", dp[2], dp[3]));
			} else {
				ND_TCHECK(dp[1]);
				/*
				 * Print the offset as signed, since -1 is
				 * common, but offsets > 2^31 aren't.
				 */
				ND_PRINT((ndo, " %u bytes @ %d",
				    EXTRACT_32BITS(&dp[1]),
				    EXTRACT_32BITS(&dp[0])));
			}
			return;
		}
		break;

	case NFSPROC_READDIRPLUS:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			ND_TCHECK(dp[4]);
			/*
			 * We don't try to interpret the offset
			 * cookie here.
			 */
			ND_PRINT((ndo, " %u bytes @ %" PRId64,
				EXTRACT_32BITS(&dp[4]),
				EXTRACT_64BITS(&dp[0])));
			if (ndo->ndo_vflag) {
				ND_TCHECK(dp[5]);
				ND_PRINT((ndo, " max %u verf %08x%08x",
				       EXTRACT_32BITS(&dp[5]), dp[2], dp[3]));
			}
			return;
		}
		break;

	case NFSPROC_COMMIT:
		if ((dp = parsereq(ndo, rp, length)) != NULL &&
		    (dp = parsefh(ndo, dp, v3)) != NULL) {
			ND_TCHECK(dp[2]);
			ND_PRINT((ndo, " %u bytes @ %" PRIu64,
				EXTRACT_32BITS(&dp[2]),
				EXTRACT_64BITS(&dp[0])));
			return;
		}
		break;

	default:
		return;
	}

trunc:
	if (!nfserr)
		ND_PRINT((ndo, "%s", tstr));
}
