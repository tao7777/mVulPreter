nfs_printfh(netdissect_options *ndo,
            register const uint32_t *dp, const u_int len)
{
	my_fsid fsid;
	uint32_t ino;
	const char *sfsname = NULL;
	char *spacep;

	if (ndo->ndo_uflag) {
		u_int i;
		char const *sep = "";

		ND_PRINT((ndo, " fh["));
		for (i=0; i<len; i++) {
			ND_PRINT((ndo, "%s%x", sep, dp[i]));
			sep = ":";
		}
		ND_PRINT((ndo, "]"));
		return;
	}

	Parse_fh((const u_char *)dp, len, &fsid, &ino, NULL, &sfsname, 0);
 
 	if (sfsname) {
 		/* file system ID is ASCII, not numeric, for this server OS */
		static char temp[NFSX_V3FHMAX+1];
 
 		/* Make sure string is null-terminated */
		strncpy(temp, sfsname, NFSX_V3FHMAX);
		temp[sizeof(temp) - 1] = '\0';
 		/* Remove trailing spaces */
 		spacep = strchr(temp, ' ');
 		if (spacep)
			*spacep = '\0';

		ND_PRINT((ndo, " fh %s/", temp));
	} else {
		ND_PRINT((ndo, " fh %d,%d/",
			     fsid.Fsid_dev.Major, fsid.Fsid_dev.Minor));
	}

	if(fsid.Fsid_dev.Minor == 257)
		/* Print the undecoded handle */
		ND_PRINT((ndo, "%s", fsid.Opaque_Handle));
	else
		ND_PRINT((ndo, "%ld", (long) ino));
}
