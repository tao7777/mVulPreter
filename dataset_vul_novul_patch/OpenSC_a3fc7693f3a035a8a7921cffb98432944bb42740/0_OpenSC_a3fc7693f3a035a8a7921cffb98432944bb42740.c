static int asn1_decode_entry(sc_context_t *ctx,struct sc_asn1_entry *entry,
			     const u8 *obj, size_t objlen, int depth)
{
	void *parm = entry->parm;
	int (*callback_func)(sc_context_t *nctx, void *arg, const u8 *nobj,
			     size_t nobjlen, int ndepth);
	size_t *len = (size_t *) entry->arg;
	int r = 0;

	callback_func = parm;

	sc_debug(ctx, SC_LOG_DEBUG_ASN1, "%*.*sdecoding '%s', raw data:%s%s\n",
		depth, depth, "", entry->name,
		sc_dump_hex(obj, objlen > 16  ? 16 : objlen),
		objlen > 16 ? "..." : "");

	switch (entry->type) {
	case SC_ASN1_STRUCT:
		if (parm != NULL)
			r = asn1_decode(ctx, (struct sc_asn1_entry *) parm, obj,
				       objlen, NULL, NULL, 0, depth + 1);
		break;
	case SC_ASN1_NULL:
		break;
	case SC_ASN1_BOOLEAN:
		if (parm != NULL) {
			if (objlen != 1) {
				sc_debug(ctx, SC_LOG_DEBUG_ASN1,
					 "invalid ASN.1 object length: %"SC_FORMAT_LEN_SIZE_T"u\n",
					 objlen);
				r = SC_ERROR_INVALID_ASN1_OBJECT;
			} else
				*((int *) parm) = obj[0] ? 1 : 0;
		}
		break;
	case SC_ASN1_INTEGER:
	case SC_ASN1_ENUMERATED:
		if (parm != NULL) {
			r = sc_asn1_decode_integer(obj, objlen, (int *) entry->parm);
			sc_debug(ctx, SC_LOG_DEBUG_ASN1, "%*.*sdecoding '%s' returned %d\n", depth, depth, "",
					entry->name, *((int *) entry->parm));
		}
		break;
	case SC_ASN1_BIT_STRING_NI:
	case SC_ASN1_BIT_STRING:
		if (parm != NULL) {
			int invert = entry->type == SC_ASN1_BIT_STRING ? 1 : 0;
			assert(len != NULL);
			if (objlen < 1) {
				r = SC_ERROR_INVALID_ASN1_OBJECT;
				break;
			}
			if (entry->flags & SC_ASN1_ALLOC) {
				u8 **buf = (u8 **) parm;
				*buf = malloc(objlen-1);
				if (*buf == NULL) {
					r = SC_ERROR_OUT_OF_MEMORY;
					break;
				}
				*len = objlen-1;
				parm = *buf;
			}
			r = decode_bit_string(obj, objlen, (u8 *) parm, *len, invert);
			if (r >= 0) {
				*len = r;
				r = 0;
			}
		}
		break;
	case SC_ASN1_BIT_FIELD:
		if (parm != NULL)
			r = decode_bit_field(obj, objlen, (u8 *) parm, *len);
		break;
	case SC_ASN1_OCTET_STRING:
		if (parm != NULL) {
			size_t c;
			assert(len != NULL);
 
 			/* Strip off padding zero */
 			if ((entry->flags & SC_ASN1_UNSIGNED)
					&& objlen > 1 && obj[0] == 0x00) {
 				objlen--;
 				obj++;
 			}

			/* Allocate buffer if needed */
			if (entry->flags & SC_ASN1_ALLOC) {
				u8 **buf = (u8 **) parm;
				*buf = malloc(objlen);
				if (*buf == NULL) {
					r = SC_ERROR_OUT_OF_MEMORY;
					break;
				}
				c = *len = objlen;
				parm = *buf;
			} else
				c = objlen > *len ? *len : objlen;

			memcpy(parm, obj, c);
			*len = c;
		}
		break;
	case SC_ASN1_GENERALIZEDTIME:
		if (parm != NULL) {
			size_t c;
			assert(len != NULL);
			if (entry->flags & SC_ASN1_ALLOC) {
				u8 **buf = (u8 **) parm;
				*buf = malloc(objlen);
				if (*buf == NULL) {
					r = SC_ERROR_OUT_OF_MEMORY;
					break;
				}
				c = *len = objlen;
				parm = *buf;
			} else
				c = objlen > *len ? *len : objlen;

			memcpy(parm, obj, c);
			*len = c;
		}
		break;
	case SC_ASN1_OBJECT:
		if (parm != NULL)
			r = sc_asn1_decode_object_id(obj, objlen, (struct sc_object_id *) parm);
		break;
	case SC_ASN1_PRINTABLESTRING:
	case SC_ASN1_UTF8STRING:
		if (parm != NULL) {
			assert(len != NULL);
			if (entry->flags & SC_ASN1_ALLOC) {
				u8 **buf = (u8 **) parm;
				*buf = malloc(objlen+1);
				if (*buf == NULL) {
					r = SC_ERROR_OUT_OF_MEMORY;
					break;
				}
				*len = objlen+1;
				parm = *buf;
			}
			r = sc_asn1_decode_utf8string(obj, objlen, (u8 *) parm, len);
			if (entry->flags & SC_ASN1_ALLOC) {
				*len -= 1;
			}
		}
		break;
	case SC_ASN1_PATH:
		if (entry->parm != NULL)
			r = asn1_decode_path(ctx, obj, objlen, (sc_path_t *) parm, depth);
		break;
	case SC_ASN1_PKCS15_ID:
		if (entry->parm != NULL) {
			struct sc_pkcs15_id *id = (struct sc_pkcs15_id *) parm;
			size_t c = objlen > sizeof(id->value) ? sizeof(id->value) : objlen;

			memcpy(id->value, obj, c);
			id->len = c;
		}
		break;
	case SC_ASN1_PKCS15_OBJECT:
		if (entry->parm != NULL)
			r = asn1_decode_p15_object(ctx, obj, objlen, (struct sc_asn1_pkcs15_object *) parm, depth);
		break;
	case SC_ASN1_ALGORITHM_ID:
		if (entry->parm != NULL)
			r = sc_asn1_decode_algorithm_id(ctx, obj, objlen, (struct sc_algorithm_id *) parm, depth);
		break;
	case SC_ASN1_SE_INFO:
		if (entry->parm != NULL)
			r = asn1_decode_se_info(ctx, obj, objlen, (sc_pkcs15_sec_env_info_t ***)entry->parm, len, depth);
		break;
	case SC_ASN1_CALLBACK:
		if (entry->parm != NULL)
			r = callback_func(ctx, entry->arg, obj, objlen, depth);
		break;
	default:
		sc_debug(ctx, SC_LOG_DEBUG_ASN1, "invalid ASN.1 type: %d\n", entry->type);
		return SC_ERROR_INVALID_ASN1_OBJECT;
	}
	if (r) {
		sc_debug(ctx, SC_LOG_DEBUG_ASN1, "decoding of ASN.1 object '%s' failed: %s\n", entry->name,
		      sc_strerror(r));
		return r;
	}
	entry->flags |= SC_ASN1_PRESENT;
	return 0;
}
