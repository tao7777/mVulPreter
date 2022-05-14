 _PUBLIC_ bool ldap_encode(struct ldap_message *msg,
			  DATA_BLOB *result, TALLOC_CTX *mem_ctx)
{
	struct asn1_data *data = asn1_init(mem_ctx);
	int i, j;

 
        if (!data) return false;
 
       if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
       if (!asn1_write_Integer(data, msg->messageid)) goto err;
 
        switch (msg->type) {
        case LDAP_TAG_BindRequest: {
                struct ldap_BindRequest *r = &msg->r.BindRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_Integer(data, r->version)) goto err;
               if (!asn1_write_OctetString(data, r->dn,
                                      (r->dn != NULL) ? strlen(r->dn) : 0)) goto err;
 
                switch (r->mechanism) {
                case LDAP_AUTH_MECH_SIMPLE:
                        /* context, primitive */
                       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0))) goto err;
                       if (!asn1_write(data, r->creds.password,
                                  strlen(r->creds.password))) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                        break;
                case LDAP_AUTH_MECH_SASL:
                        /* context, constructed */
                       if (!asn1_push_tag(data, ASN1_CONTEXT(3))) goto err;
                       if (!asn1_write_OctetString(data, r->creds.SASL.mechanism,
                                              strlen(r->creds.SASL.mechanism))) goto err;
                        if (r->creds.SASL.secblob) {
                               if (!asn1_write_OctetString(data, r->creds.SASL.secblob->data,
                                                      r->creds.SASL.secblob->length)) goto err;
                        }
                       if (!asn1_pop_tag(data)) goto err;
                        break;
                default:
                       goto err;
                }
 
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_BindResponse: {
                struct ldap_BindResponse *r = &msg->r.BindResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, &r->response)) goto err;
                if (r->SASL.secblob) {
                       if (!asn1_write_ContextSimple(data, 7, r->SASL.secblob)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_UnbindRequest: {
 /*             struct ldap_UnbindRequest *r = &msg->r.UnbindRequest; */
               if (!asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type))) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_SearchRequest: {
                struct ldap_SearchRequest *r = &msg->r.SearchRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->basedn, strlen(r->basedn))) goto err;
               if (!asn1_write_enumerated(data, r->scope)) goto err;
               if (!asn1_write_enumerated(data, r->deref)) goto err;
               if (!asn1_write_Integer(data, r->sizelimit)) goto err;
               if (!asn1_write_Integer(data, r->timelimit)) goto err;
               if (!asn1_write_BOOLEAN(data, r->attributesonly)) goto err;
 
                if (!ldap_push_filter(data, r->tree)) {
                       goto err;
                }
 
               if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                for (i=0; i<r->num_attributes; i++) {
                       if (!asn1_write_OctetString(data, r->attributes[i],
                                              strlen(r->attributes[i]))) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_SearchResultEntry: {
                struct ldap_SearchResEntry *r = &msg->r.SearchResultEntry;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                for (i=0; i<r->num_attributes; i++) {
                        struct ldb_message_element *attr = &r->attributes[i];
                       if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                       if (!asn1_write_OctetString(data, attr->name,
                                              strlen(attr->name))) goto err;
                       if (!asn1_push_tag(data, ASN1_SEQUENCE(1))) goto err;
                        for (j=0; j<attr->num_values; j++) {
                               if (!asn1_write_OctetString(data,
                                                       attr->values[j].data,
                                                      attr->values[j].length)) goto err;
                        }
                       if (!asn1_pop_tag(data)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_SearchResultDone: {
                struct ldap_Result *r = &msg->r.SearchResultDone;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ModifyRequest: {
                struct ldap_ModifyRequest *r = &msg->r.ModifyRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
 
                for (i=0; i<r->num_mods; i++) {
                        struct ldb_message_element *attrib = &r->mods[i].attrib;
                       if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                       if (!asn1_write_enumerated(data, r->mods[i].type)) goto err;
                       if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                       if (!asn1_write_OctetString(data, attrib->name,
                                              strlen(attrib->name))) goto err;
                       if (!asn1_push_tag(data, ASN1_SET)) goto err;
                        for (j=0; j<attrib->num_values; j++) {
                               if (!asn1_write_OctetString(data,
                                                       attrib->values[j].data,
                                                      attrib->values[j].length)) goto err;
        
                        }
                       if (!asn1_pop_tag(data)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
                
               if (!asn1_pop_tag(data)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ModifyResponse: {
                struct ldap_Result *r = &msg->r.ModifyResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_AddRequest: {
                struct ldap_AddRequest *r = &msg->r.AddRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
 
                for (i=0; i<r->num_attributes; i++) {
                        struct ldb_message_element *attrib = &r->attributes[i];
                       if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
                       if (!asn1_write_OctetString(data, attrib->name,
                                              strlen(attrib->name))) goto err;
                       if (!asn1_push_tag(data, ASN1_SET)) goto err;
                        for (j=0; j<r->attributes[i].num_values; j++) {
                               if (!asn1_write_OctetString(data,
                                                       attrib->values[j].data,
                                                      attrib->values[j].length)) goto err;
                        }
                       if (!asn1_pop_tag(data)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_AddResponse: {
                struct ldap_Result *r = &msg->r.AddResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_DelRequest: {
                struct ldap_DelRequest *r = &msg->r.DelRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type))) goto err;
               if (!asn1_write(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_DelResponse: {
                struct ldap_Result *r = &msg->r.DelResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ModifyDNRequest: {
                struct ldap_ModifyDNRequest *r = &msg->r.ModifyDNRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_write_OctetString(data, r->newrdn, strlen(r->newrdn))) goto err;
               if (!asn1_write_BOOLEAN(data, r->deleteolddn)) goto err;
                if (r->newsuperior) {
                       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0))) goto err;
                       if (!asn1_write(data, r->newsuperior,
                                  strlen(r->newsuperior))) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ModifyDNResponse: {
                struct ldap_Result *r = &msg->r.ModifyDNResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_CompareRequest: {
                struct ldap_CompareRequest *r = &msg->r.CompareRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->dn, strlen(r->dn))) goto err;
               if (!asn1_push_tag(data, ASN1_SEQUENCE(0))) goto err;
               if (!asn1_write_OctetString(data, r->attribute,
                                      strlen(r->attribute))) goto err;
               if (!asn1_write_OctetString(data, r->value.data,
                                      r->value.length)) goto err;
               if (!asn1_pop_tag(data)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_CompareResponse: {
                struct ldap_Result *r = &msg->r.ModifyDNResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, r)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_AbandonRequest: {
                struct ldap_AbandonRequest *r = &msg->r.AbandonRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type))) goto err;
               if (!asn1_write_implicit_Integer(data, r->messageid)) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_SearchResultReference: {
                struct ldap_SearchResRef *r = &msg->r.SearchResultReference;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_write_OctetString(data, r->referral, strlen(r->referral))) goto err;
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ExtendedRequest: {
                struct ldap_ExtendedRequest *r = &msg->r.ExtendedRequest;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0))) goto err;
               if (!asn1_write(data, r->oid, strlen(r->oid))) goto err;
               if (!asn1_pop_tag(data)) goto err;
                if (r->value) {
                       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(1))) goto err;
                       if (!asn1_write(data, r->value->data, r->value->length)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        case LDAP_TAG_ExtendedResponse: {
                struct ldap_ExtendedResponse *r = &msg->r.ExtendedResponse;
               if (!asn1_push_tag(data, ASN1_APPLICATION(msg->type))) goto err;
               if (!ldap_encode_response(data, &r->response)) goto err;
                if (r->oid) {
                       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(10))) goto err;
                       if (!asn1_write(data, r->oid, strlen(r->oid))) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
                if (r->value) {
                       if (!asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(11))) goto err;
                       if (!asn1_write(data, r->value->data, r->value->length)) goto err;
                       if (!asn1_pop_tag(data)) goto err;
                }
               if (!asn1_pop_tag(data)) goto err;
                break;
        }
        default:
               goto err;
        }
 
        if (msg->controls != NULL) {
               if (!asn1_push_tag(data, ASN1_CONTEXT(0))) goto err;
                
                for (i = 0; msg->controls[i] != NULL; i++) {
                        if (!ldap_encode_control(mem_ctx, data,
						 msg->controls[i])) {
                                                 msg->controls[i])) {
                                DEBUG(0,("Unable to encode control %s\n",
                                         msg->controls[i]->oid));
                               goto err;
                        }
                }
 
               if (!asn1_pop_tag(data)) goto err;
        }
 
       if (!asn1_pop_tag(data)) goto err;
 
        *result = data_blob_talloc(mem_ctx, data->data, data->length);
        asn1_free(data);

        return true;

  err:

       asn1_free(data);
       return false;
 }
 
 static const char *blob2string_talloc(TALLOC_CTX *mem_ctx,
	char *result = talloc_array(mem_ctx, char, blob.length+1);
	memcpy(result, blob.data, blob.length);
	result[blob.length] = '\0';
	return result;
}
