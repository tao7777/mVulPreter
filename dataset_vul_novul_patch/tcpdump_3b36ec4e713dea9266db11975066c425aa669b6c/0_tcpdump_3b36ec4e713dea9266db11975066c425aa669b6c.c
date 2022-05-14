vqp_print(netdissect_options *ndo, register const u_char *pptr, register u_int len)
{
    const struct vqp_common_header_t *vqp_common_header;
    const struct vqp_obj_tlv_t *vqp_obj_tlv;

     const u_char *tptr;
     uint16_t vqp_obj_len;
     uint32_t vqp_obj_type;
    u_int tlen;
     uint8_t nitems;
 
     tptr=pptr;
     tlen = len;
     vqp_common_header = (const struct vqp_common_header_t *)pptr;
     ND_TCHECK(*vqp_common_header);
    if (sizeof(struct vqp_common_header_t) > tlen)
        goto trunc;
 
     /*
      * Sanity checking of the header.
     */
    if (VQP_EXTRACT_VERSION(vqp_common_header->version) != VQP_VERSION) {
	ND_PRINT((ndo, "VQP version %u packet not supported",
               VQP_EXTRACT_VERSION(vqp_common_header->version)));
	return;
    }

    /* in non-verbose mode just lets print the basic Message Type */
    if (ndo->ndo_vflag < 1) {
        ND_PRINT((ndo, "VQPv%u %s Message, error-code %s (%u), length %u",
               VQP_EXTRACT_VERSION(vqp_common_header->version),
               tok2str(vqp_msg_type_values, "unknown (%u)",vqp_common_header->msg_type),
               tok2str(vqp_error_code_values, "unknown (%u)",vqp_common_header->error_code),
	       vqp_common_header->error_code,
               len));
        return;
    }

    /* ok they seem to want to know everything - lets fully decode it */
    nitems = vqp_common_header->nitems;
    ND_PRINT((ndo, "\n\tVQPv%u, %s Message, error-code %s (%u), seq 0x%08x, items %u, length %u",
           VQP_EXTRACT_VERSION(vqp_common_header->version),
	   tok2str(vqp_msg_type_values, "unknown (%u)",vqp_common_header->msg_type),
	   tok2str(vqp_error_code_values, "unknown (%u)",vqp_common_header->error_code),
	   vqp_common_header->error_code,
           EXTRACT_32BITS(&vqp_common_header->sequence),
           nitems,
           len));

    /* skip VQP Common header */
    tptr+=sizeof(const struct vqp_common_header_t);
    tlen-=sizeof(const struct vqp_common_header_t);

     while (nitems > 0 && tlen > 0) {
 
         vqp_obj_tlv = (const struct vqp_obj_tlv_t *)tptr;
        ND_TCHECK(*vqp_obj_tlv);
        if (sizeof(struct vqp_obj_tlv_t) > tlen)
            goto trunc;
         vqp_obj_type = EXTRACT_32BITS(vqp_obj_tlv->obj_type);
         vqp_obj_len = EXTRACT_16BITS(vqp_obj_tlv->obj_length);
         tptr+=sizeof(struct vqp_obj_tlv_t);
        tlen-=sizeof(struct vqp_obj_tlv_t);

        ND_PRINT((ndo, "\n\t  %s Object (0x%08x), length %u, value: ",
               tok2str(vqp_obj_values, "Unknown", vqp_obj_type),
               vqp_obj_type, vqp_obj_len));

        /* basic sanity check */
        if (vqp_obj_type == 0 || vqp_obj_len ==0) {
            return;
        }
 
         /* did we capture enough for fully decoding the object ? */
         ND_TCHECK2(*tptr, vqp_obj_len);
        if (vqp_obj_len > tlen)
            goto trunc;
 
         switch(vqp_obj_type) {
 	case VQP_OBJ_IP_ADDRESS:
            if (vqp_obj_len != 4)
                goto trunc;
             ND_PRINT((ndo, "%s (0x%08x)", ipaddr_string(ndo, tptr), EXTRACT_32BITS(tptr)));
             break;
             /* those objects have similar semantics - fall through */
        case VQP_OBJ_PORT_NAME:
	case VQP_OBJ_VLAN_NAME:
	case VQP_OBJ_VTP_DOMAIN:
	case VQP_OBJ_ETHERNET_PKT:
            safeputs(ndo, tptr, vqp_obj_len);
            break;
             /* those objects have similar semantics - fall through */
 	case VQP_OBJ_MAC_ADDRESS:
 	case VQP_OBJ_MAC_NULL:
            if (vqp_obj_len != ETHER_ADDR_LEN)
                goto trunc;
 	      ND_PRINT((ndo, "%s", etheraddr_string(ndo, tptr)));
               break;
         default:
            if (ndo->ndo_vflag <= 1)
                print_unknown_data(ndo,tptr, "\n\t    ", vqp_obj_len);
            break;
        }
	tptr += vqp_obj_len;
	tlen -= vqp_obj_len;
	nitems--;
    }
    return;
trunc:
    ND_PRINT((ndo, "\n\t[|VQP]"));
}
