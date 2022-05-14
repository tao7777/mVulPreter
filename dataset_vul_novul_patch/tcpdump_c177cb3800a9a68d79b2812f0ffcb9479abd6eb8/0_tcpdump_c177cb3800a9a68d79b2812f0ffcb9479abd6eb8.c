esis_print(netdissect_options *ndo,
           const uint8_t *pptr, u_int length)
{
	const uint8_t *optr;
	u_int li,esis_pdu_type,source_address_length, source_address_number;
	const struct esis_header_t *esis_header;

	if (!ndo->ndo_eflag)
		ND_PRINT((ndo, "ES-IS"));

	if (length <= 2) {
		ND_PRINT((ndo, ndo->ndo_qflag ? "bad pkt!" : "no header at all!"));
		return;
	}

	esis_header = (const struct esis_header_t *) pptr;
        ND_TCHECK(*esis_header);
        li = esis_header->length_indicator;
        optr = pptr;

        /*
         * Sanity checking of the header.
         */

        if (esis_header->nlpid != NLPID_ESIS) {
            ND_PRINT((ndo, " nlpid 0x%02x packet not supported", esis_header->nlpid));
            return;
        }

        if (esis_header->version != ESIS_VERSION) {
            ND_PRINT((ndo, " version %d packet not supported", esis_header->version));
            return;
        }

	if (li > length) {
            ND_PRINT((ndo, " length indicator(%u) > PDU size (%u)!", li, length));
            return;
	}

	if (li < sizeof(struct esis_header_t) + 2) {
            ND_PRINT((ndo, " length indicator %u < min PDU size:", li));
            while (pptr < ndo->ndo_snapend)
                ND_PRINT((ndo, "%02X", *pptr++));
            return;
	}

        esis_pdu_type = esis_header->type & ESIS_PDU_TYPE_MASK;

        if (ndo->ndo_vflag < 1) {
            ND_PRINT((ndo, "%s%s, length %u",
                   ndo->ndo_eflag ? "" : ", ",
                   tok2str(esis_pdu_values,"unknown type (%u)",esis_pdu_type),
                   length));
            return;
        } else
            ND_PRINT((ndo, "%slength %u\n\t%s (%u)",
                   ndo->ndo_eflag ? "" : ", ",
                   length,
                   tok2str(esis_pdu_values,"unknown type: %u", esis_pdu_type),
                   esis_pdu_type));

        ND_PRINT((ndo, ", v: %u%s", esis_header->version, esis_header->version == ESIS_VERSION ? "" : "unsupported" ));
        ND_PRINT((ndo, ", checksum: 0x%04x", EXTRACT_16BITS(esis_header->cksum)));

        osi_print_cksum(ndo, pptr, EXTRACT_16BITS(esis_header->cksum), 7, li);

        ND_PRINT((ndo, ", holding time: %us, length indicator: %u",
                  EXTRACT_16BITS(esis_header->holdtime), li));

        if (ndo->ndo_vflag > 1)
            print_unknown_data(ndo, optr, "\n\t", sizeof(struct esis_header_t));

	pptr += sizeof(struct esis_header_t);
	li -= sizeof(struct esis_header_t);

	switch (esis_pdu_type) {
	case ESIS_PDU_REDIRECT: {
		const uint8_t *dst, *snpa, *neta;
		u_int dstl, snpal, netal;

		ND_TCHECK(*pptr);
		if (li < 1) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		dstl = *pptr;
		pptr++;
		li--;
		ND_TCHECK2(*pptr, dstl);
		if (li < dstl) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		dst = pptr;
		pptr += dstl;
                li -= dstl;
		ND_PRINT((ndo, "\n\t  %s", isonsap_string(ndo, dst, dstl)));

		ND_TCHECK(*pptr);
		if (li < 1) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		snpal = *pptr;
		pptr++;
		li--;
		ND_TCHECK2(*pptr, snpal);
		if (li < snpal) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		snpa = pptr;
		pptr += snpal;
                li -= snpal;
		ND_TCHECK(*pptr);
		if (li < 1) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		netal = *pptr;
		pptr++;
		ND_TCHECK2(*pptr, netal);
		if (li < netal) {
			ND_PRINT((ndo, ", bad redirect/li"));
			return;
		}
		neta = pptr;
 		pptr += netal;
                 li -= netal;
 
		if (snpal == 6)
			ND_PRINT((ndo, "\n\t  SNPA (length: %u): %s",
			       snpal,
			       etheraddr_string(ndo, snpa)));
 		else
			ND_PRINT((ndo, "\n\t  SNPA (length: %u): %s",
			       snpal,
			       linkaddr_string(ndo, snpa, LINKADDR_OTHER, snpal)));
		if (netal != 0)
			ND_PRINT((ndo, "\n\t  NET (length: %u) %s",
			       netal,
			       isonsap_string(ndo, neta, netal)));
 		break;
 	}
 
	case ESIS_PDU_ESH:
            ND_TCHECK(*pptr);
            if (li < 1) {
                ND_PRINT((ndo, ", bad esh/li"));
                return;
            }
            source_address_number = *pptr;
            pptr++;
            li--;

            ND_PRINT((ndo, "\n\t  Number of Source Addresses: %u", source_address_number));

            while (source_address_number > 0) {
                ND_TCHECK(*pptr);
            	if (li < 1) {
                    ND_PRINT((ndo, ", bad esh/li"));
            	    return;
            	}
                source_address_length = *pptr;
                pptr++;
            	li--;

                ND_TCHECK2(*pptr, source_address_length);
            	if (li < source_address_length) {
                    ND_PRINT((ndo, ", bad esh/li"));
            	    return;
            	}
                ND_PRINT((ndo, "\n\t  NET (length: %u): %s",
                       source_address_length,
                       isonsap_string(ndo, pptr, source_address_length)));
                pptr += source_address_length;
                li -= source_address_length;
                source_address_number--;
            }

            break;

	case ESIS_PDU_ISH: {
            ND_TCHECK(*pptr);
            if (li < 1) {
                ND_PRINT((ndo, ", bad ish/li"));
                return;
            }
            source_address_length = *pptr;
            pptr++;
            li--;
            ND_TCHECK2(*pptr, source_address_length);
            if (li < source_address_length) {
                ND_PRINT((ndo, ", bad ish/li"));
                return;
            }
            ND_PRINT((ndo, "\n\t  NET (length: %u): %s", source_address_length, isonsap_string(ndo, pptr, source_address_length)));
            pptr += source_address_length;
            li -= source_address_length;
            break;
	}

	default:
		if (ndo->ndo_vflag <= 1) {
			if (pptr < ndo->ndo_snapend)
				print_unknown_data(ndo, pptr, "\n\t  ", ndo->ndo_snapend - pptr);
		}
		return;
	}

        /* now walk the options */
        while (li != 0) {
            u_int op, opli;
            const uint8_t *tptr;

            if (li < 2) {
                ND_PRINT((ndo, ", bad opts/li"));
                return;
            }
            ND_TCHECK2(*pptr, 2);
            op = *pptr++;
            opli = *pptr++;
            li -= 2;
            if (opli > li) {
                ND_PRINT((ndo, ", opt (%d) too long", op));
                return;
            }
            li -= opli;
            tptr = pptr;

            ND_PRINT((ndo, "\n\t  %s Option #%u, length %u, value: ",
                   tok2str(esis_option_values,"Unknown",op),
                   op,
                   opli));

            switch (op) {

            case ESIS_OPTION_ES_CONF_TIME:
                if (opli == 2) {
                    ND_TCHECK2(*pptr, 2);
                    ND_PRINT((ndo, "%us", EXTRACT_16BITS(tptr)));
                } else
                    ND_PRINT((ndo, "(bad length)"));
                break;

            case ESIS_OPTION_PROTOCOLS:
                while (opli>0) {
                    ND_TCHECK(*pptr);
                    ND_PRINT((ndo, "%s (0x%02x)",
                           tok2str(nlpid_values,
                                   "unknown",
                                   *tptr),
                           *tptr));
                    if (opli>1) /* further NPLIDs ? - put comma */
                        ND_PRINT((ndo, ", "));
                    tptr++;
                    opli--;
                }
                break;

                /*
                 * FIXME those are the defined Options that lack a decoder
                 * you are welcome to contribute code ;-)
                 */

            case ESIS_OPTION_QOS_MAINTENANCE:
            case ESIS_OPTION_SECURITY:
            case ESIS_OPTION_PRIORITY:
            case ESIS_OPTION_ADDRESS_MASK:
            case ESIS_OPTION_SNPA_MASK:

            default:
                print_unknown_data(ndo, tptr, "\n\t  ", opli);
                break;
            }
            if (ndo->ndo_vflag > 1)
                print_unknown_data(ndo, pptr, "\n\t  ", opli);
            pptr += opli;
        }
trunc:
	return;
}
