ubik_print(netdissect_options *ndo,
           register const u_char *bp)
{
	int ubik_op;
	int32_t temp;

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from ubik/ubik_int.xg
	 */

	ubik_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " ubik call %s", tok2str(ubik_req, "op#%d", ubik_op)));

	/*
	 * Decode some of the arguments to the Ubik calls
	 */

	bp += sizeof(struct rx_header) + 4;

	switch (ubik_op) {
		case 10000:		/* Beacon */
			ND_TCHECK2(bp[0], 4);
			temp = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);
			ND_PRINT((ndo, " syncsite %s", temp ? "yes" : "no"));
			ND_PRINT((ndo, " votestart"));
			DATEOUT();
			ND_PRINT((ndo, " dbversion"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			break;
		case 10003:		/* Get sync site */
			ND_PRINT((ndo, " site"));
			UINTOUT();
			break;
		case 20000:		/* Begin */
		case 20001:		/* Commit */
		case 20007:		/* Abort */
		case 20008:		/* Release locks */
		case 20010:		/* Writev */
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			break;
		case 20002:		/* Lock */
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " file"));
			INTOUT();
			ND_PRINT((ndo, " pos"));
 			INTOUT();
 			ND_PRINT((ndo, " length"));
 			INTOUT();
			ND_TCHECK_32BITS(bp);
 			temp = EXTRACT_32BITS(bp);
 			bp += sizeof(int32_t);
 			tok2str(ubik_lock_types, "type %d", temp);
			break;
		case 20003:		/* Write */
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " file"));
			INTOUT();
			ND_PRINT((ndo, " pos"));
			INTOUT();
			break;
		case 20005:		/* Get file */
			ND_PRINT((ndo, " file"));
			INTOUT();
			break;
		case 20006:		/* Send file */
			ND_PRINT((ndo, " file"));
			INTOUT();
			ND_PRINT((ndo, " length"));
			INTOUT();
			ND_PRINT((ndo, " dbversion"));
			UBIK_VERSIONOUT();
			break;
		case 20009:		/* Truncate */
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " file"));
			INTOUT();
			ND_PRINT((ndo, " length"));
			INTOUT();
			break;
		case 20012:		/* Set version */
			ND_PRINT((ndo, " tid"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " oldversion"));
			UBIK_VERSIONOUT();
			ND_PRINT((ndo, " newversion"));
			UBIK_VERSIONOUT();
			break;
		default:
			;
	}

	return;

trunc:
	ND_PRINT((ndo, " [|ubik]"));
}
