lldp_private_8023_print(netdissect_options *ndo,
                        const u_char *tptr, u_int tlv_len)
{
    int subtype, hexdump = FALSE;

    if (tlv_len < 4) {
        return hexdump;
    }
    subtype = *(tptr+3);

    ND_PRINT((ndo, "\n\t  %s Subtype (%u)",
           tok2str(lldp_8023_subtype_values, "unknown", subtype),
           subtype));

    switch (subtype) {
    case LLDP_PRIVATE_8023_SUBTYPE_MACPHY:
        if (tlv_len < 9) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    autonegotiation [%s] (0x%02x)",
               bittok2str(lldp_8023_autonegotiation_values, "none", *(tptr+4)),
               *(tptr + 4)));
        ND_PRINT((ndo, "\n\t    PMD autoneg capability [%s] (0x%04x)",
               bittok2str(lldp_pmd_capability_values,"unknown", EXTRACT_16BITS(tptr+5)),
               EXTRACT_16BITS(tptr + 5)));
        ND_PRINT((ndo, "\n\t    MAU type %s (0x%04x)",
               tok2str(lldp_mau_types_values, "unknown", EXTRACT_16BITS(tptr+7)),
               EXTRACT_16BITS(tptr + 7)));
        break;

    case LLDP_PRIVATE_8023_SUBTYPE_MDIPOWER:
        if (tlv_len < 7) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    MDI power support [%s], power pair %s, power class %s",
               bittok2str(lldp_mdi_values, "none", *(tptr+4)),
               tok2str(lldp_mdi_power_pairs_values, "unknown", *(tptr+5)),
               tok2str(lldp_mdi_power_class_values, "unknown", *(tptr + 6))));
        break;

    case LLDP_PRIVATE_8023_SUBTYPE_LINKAGGR:
        if (tlv_len < 9) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    aggregation status [%s], aggregation port ID %u",
               bittok2str(lldp_aggregation_values, "none", *(tptr+4)),
               EXTRACT_32BITS(tptr + 5)));
         break;
 
     case LLDP_PRIVATE_8023_SUBTYPE_MTU:
        if (tlv_len < 6) {
            return hexdump;
        }
         ND_PRINT((ndo, "\n\t    MTU size %u", EXTRACT_16BITS(tptr + 4)));
         break;
 
    default:
        hexdump = TRUE;
        break;
    }

    return hexdump;
}
