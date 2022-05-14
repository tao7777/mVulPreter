aodv_extension(netdissect_options *ndo,
               const struct aodv_ext *ep, u_int length)
 {
 	const struct aodv_hello *ah;
 
 	switch (ep->type) {
 	case AODV_EXT_HELLO:
 		ah = (const struct aodv_hello *)(const void *)ep;
 		ND_TCHECK(*ah);
 		if (length < sizeof(struct aodv_hello))
 			goto trunc;
 		ND_PRINT((ndo, "\n\text HELLO %ld ms",
 		    (unsigned long)EXTRACT_32BITS(&ah->interval)));
 		break;

	default:
		ND_PRINT((ndo, "\n\text %u %u", ep->type, ep->length));
		break;
	}
	return;

trunc:
	ND_PRINT((ndo, " [|hello]"));
}
