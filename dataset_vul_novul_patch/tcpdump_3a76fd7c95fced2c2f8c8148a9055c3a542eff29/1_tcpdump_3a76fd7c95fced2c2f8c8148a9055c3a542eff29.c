ns_nprint(netdissect_options *ndo,
          register const u_char *cp, register const u_char *bp)
{
 	register u_int i, l;
 	register const u_char *rp = NULL;
 	register int compress = 0;
	int chars_processed;
 	int elt;
	int data_size = ndo->ndo_snapend - bp;
 
 	if ((l = labellen(ndo, cp)) == (u_int)-1)
 		return(NULL);
 	if (!ND_TTEST2(*cp, 1))
 		return(NULL);
	chars_processed = 1;
 	if (((i = *cp++) & INDIR_MASK) != INDIR_MASK) {
 		compress = 0;
 		rp = cp + l;
	}

	if (i != 0)
		while (i && cp < ndo->ndo_snapend) {
			if ((i & INDIR_MASK) == INDIR_MASK) {
				if (!compress) {
					rp = cp + 1;
					compress = 1;
 				}
 				if (!ND_TTEST2(*cp, 1))
 					return(NULL);
				cp = bp + (((i << 8) | *cp) & 0x3fff);
 				if ((l = labellen(ndo, cp)) == (u_int)-1)
 					return(NULL);
 				if (!ND_TTEST2(*cp, 1))
 					return(NULL);
 				i = *cp++;
				chars_processed++;
				/*
				 * If we've looked at every character in
				 * the message, this pointer will make
				 * us look at some character again,
				 * which means we're looping.
				 */
				if (chars_processed >= data_size) {
					ND_PRINT((ndo, "<LOOP>"));
					return (NULL);
				}
 				continue;
 			}
 			if ((i & INDIR_MASK) == EDNS0_MASK) {
				elt = (i & ~INDIR_MASK);
				switch(elt) {
				case EDNS0_ELT_BITLABEL:
					if (blabel_print(ndo, cp) == NULL)
						return (NULL);
					break;
				default:
					/* unknown ELT */
					ND_PRINT((ndo, "<ELT %d>", elt));
					return(NULL);
				}
			} else {
				if (fn_printn(ndo, cp, l, ndo->ndo_snapend))
					return(NULL);
 			}
 
 			cp += l;
			chars_processed += l;
 			ND_PRINT((ndo, "."));
 			if ((l = labellen(ndo, cp)) == (u_int)-1)
 				return(NULL);
 			if (!ND_TTEST2(*cp, 1))
 				return(NULL);
 			i = *cp++;
			chars_processed++;
 			if (!compress)
 				rp += l + 1;
 		}
	else
		ND_PRINT((ndo, "."));
	return (rp);
}
