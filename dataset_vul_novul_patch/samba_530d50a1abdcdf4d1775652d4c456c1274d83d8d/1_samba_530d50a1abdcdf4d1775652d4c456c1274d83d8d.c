 _PUBLIC_ bool ldap_encode(struct ldap_message *msg,
			  DATA_BLOB *result, TALLOC_CTX *mem_ctx)
{
	struct asn1_data *data = asn1_init(mem_ctx);
	int i, j;

 
        if (!data) return false;
 
       asn1_push_tag(data, ASN1_SEQUENCE(0));
       asn1_write_Integer(data, msg->messageid);
 
        switch (msg->type) {
        case LDAP_TAG_BindRequest: {
                struct ldap_BindRequest *r = &msg->r.BindRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_Integer(data, r->version);
               asn1_write_OctetString(data, r->dn,
                                      (r->dn != NULL) ? strlen(r->dn) : 0);
 
                switch (r->mechanism) {
                case LDAP_AUTH_MECH_SIMPLE:
                        /* context, primitive */
                       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0));
                       asn1_write(data, r->creds.password,
                                  strlen(r->creds.password));
                       asn1_pop_tag(data);
                        break;
                case LDAP_AUTH_MECH_SASL:
                        /* context, constructed */
                       asn1_push_tag(data, ASN1_CONTEXT(3));
                       asn1_write_OctetString(data, r->creds.SASL.mechanism,
                                              strlen(r->creds.SASL.mechanism));
                        if (r->creds.SASL.secblob) {
                               asn1_write_OctetString(data, r->creds.SASL.secblob->data,
                                                      r->creds.SASL.secblob->length);
                        }
                       asn1_pop_tag(data);
                        break;
                default:
                       return false;
                }
 
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_BindResponse: {
                struct ldap_BindResponse *r = &msg->r.BindResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, &r->response);
                if (r->SASL.secblob) {
                       asn1_write_ContextSimple(data, 7, r->SASL.secblob);
                }
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_UnbindRequest: {
 /*             struct ldap_UnbindRequest *r = &msg->r.UnbindRequest; */
               asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type));
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_SearchRequest: {
                struct ldap_SearchRequest *r = &msg->r.SearchRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->basedn, strlen(r->basedn));
               asn1_write_enumerated(data, r->scope);
               asn1_write_enumerated(data, r->deref);
               asn1_write_Integer(data, r->sizelimit);
               asn1_write_Integer(data, r->timelimit);
               asn1_write_BOOLEAN(data, r->attributesonly);
 
                if (!ldap_push_filter(data, r->tree)) {
                       return false;
                }
 
               asn1_push_tag(data, ASN1_SEQUENCE(0));
                for (i=0; i<r->num_attributes; i++) {
                       asn1_write_OctetString(data, r->attributes[i],
                                              strlen(r->attributes[i]));
                }
               asn1_pop_tag(data);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_SearchResultEntry: {
                struct ldap_SearchResEntry *r = &msg->r.SearchResultEntry;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->dn, strlen(r->dn));
               asn1_push_tag(data, ASN1_SEQUENCE(0));
                for (i=0; i<r->num_attributes; i++) {
                        struct ldb_message_element *attr = &r->attributes[i];
                       asn1_push_tag(data, ASN1_SEQUENCE(0));
                       asn1_write_OctetString(data, attr->name,
                                              strlen(attr->name));
                       asn1_push_tag(data, ASN1_SEQUENCE(1));
                        for (j=0; j<attr->num_values; j++) {
                               asn1_write_OctetString(data,
                                                       attr->values[j].data,
                                                      attr->values[j].length);
                        }
                       asn1_pop_tag(data);
                       asn1_pop_tag(data);
                }
               asn1_pop_tag(data);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_SearchResultDone: {
                struct ldap_Result *r = &msg->r.SearchResultDone;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ModifyRequest: {
                struct ldap_ModifyRequest *r = &msg->r.ModifyRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->dn, strlen(r->dn));
               asn1_push_tag(data, ASN1_SEQUENCE(0));
 
                for (i=0; i<r->num_mods; i++) {
                        struct ldb_message_element *attrib = &r->mods[i].attrib;
                       asn1_push_tag(data, ASN1_SEQUENCE(0));
                       asn1_write_enumerated(data, r->mods[i].type);
                       asn1_push_tag(data, ASN1_SEQUENCE(0));
                       asn1_write_OctetString(data, attrib->name,
                                              strlen(attrib->name));
                       asn1_push_tag(data, ASN1_SET);
                        for (j=0; j<attrib->num_values; j++) {
                               asn1_write_OctetString(data,
                                                       attrib->values[j].data,
                                                      attrib->values[j].length);
        
                        }
                       asn1_pop_tag(data);
                       asn1_pop_tag(data);
                       asn1_pop_tag(data);
                }
                
               asn1_pop_tag(data);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ModifyResponse: {
                struct ldap_Result *r = &msg->r.ModifyResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_AddRequest: {
                struct ldap_AddRequest *r = &msg->r.AddRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->dn, strlen(r->dn));
               asn1_push_tag(data, ASN1_SEQUENCE(0));
 
                for (i=0; i<r->num_attributes; i++) {
                        struct ldb_message_element *attrib = &r->attributes[i];
                       asn1_push_tag(data, ASN1_SEQUENCE(0));
                       asn1_write_OctetString(data, attrib->name,
                                              strlen(attrib->name));
                       asn1_push_tag(data, ASN1_SET);
                        for (j=0; j<r->attributes[i].num_values; j++) {
                               asn1_write_OctetString(data,
                                                       attrib->values[j].data,
                                                      attrib->values[j].length);
                        }
                       asn1_pop_tag(data);
                       asn1_pop_tag(data);
                }
               asn1_pop_tag(data);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_AddResponse: {
                struct ldap_Result *r = &msg->r.AddResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_DelRequest: {
                struct ldap_DelRequest *r = &msg->r.DelRequest;
               asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type));
               asn1_write(data, r->dn, strlen(r->dn));
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_DelResponse: {
                struct ldap_Result *r = &msg->r.DelResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ModifyDNRequest: {
                struct ldap_ModifyDNRequest *r = &msg->r.ModifyDNRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->dn, strlen(r->dn));
               asn1_write_OctetString(data, r->newrdn, strlen(r->newrdn));
               asn1_write_BOOLEAN(data, r->deleteolddn);
                if (r->newsuperior) {
                       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0));
                       asn1_write(data, r->newsuperior,
                                  strlen(r->newsuperior));
                       asn1_pop_tag(data);
                }
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ModifyDNResponse: {
                struct ldap_Result *r = &msg->r.ModifyDNResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_CompareRequest: {
                struct ldap_CompareRequest *r = &msg->r.CompareRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->dn, strlen(r->dn));
               asn1_push_tag(data, ASN1_SEQUENCE(0));
               asn1_write_OctetString(data, r->attribute,
                                      strlen(r->attribute));
               asn1_write_OctetString(data, r->value.data,
                                      r->value.length);
               asn1_pop_tag(data);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_CompareResponse: {
                struct ldap_Result *r = &msg->r.ModifyDNResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, r);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_AbandonRequest: {
                struct ldap_AbandonRequest *r = &msg->r.AbandonRequest;
               asn1_push_tag(data, ASN1_APPLICATION_SIMPLE(msg->type));
               asn1_write_implicit_Integer(data, r->messageid);
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_SearchResultReference: {
                struct ldap_SearchResRef *r = &msg->r.SearchResultReference;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_write_OctetString(data, r->referral, strlen(r->referral));
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ExtendedRequest: {
                struct ldap_ExtendedRequest *r = &msg->r.ExtendedRequest;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0));
               asn1_write(data, r->oid, strlen(r->oid));
               asn1_pop_tag(data);
                if (r->value) {
                       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(1));
                       asn1_write(data, r->value->data, r->value->length);
                       asn1_pop_tag(data);
                }
               asn1_pop_tag(data);
                break;
        }
        case LDAP_TAG_ExtendedResponse: {
                struct ldap_ExtendedResponse *r = &msg->r.ExtendedResponse;
               asn1_push_tag(data, ASN1_APPLICATION(msg->type));
               ldap_encode_response(data, &r->response);
                if (r->oid) {
                       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(10));
                       asn1_write(data, r->oid, strlen(r->oid));
                       asn1_pop_tag(data);
                }
                if (r->value) {
                       asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(11));
                       asn1_write(data, r->value->data, r->value->length);
                       asn1_pop_tag(data);
                }
               asn1_pop_tag(data);
                break;
        }
        default:
               return false;
        }
 
        if (msg->controls != NULL) {
               asn1_push_tag(data, ASN1_CONTEXT(0));
                
                for (i = 0; msg->controls[i] != NULL; i++) {
                        if (!ldap_encode_control(mem_ctx, data,
						 msg->controls[i])) {
                                                 msg->controls[i])) {
                                DEBUG(0,("Unable to encode control %s\n",
                                         msg->controls[i]->oid));
                               return false;
                        }
                }
 
               asn1_pop_tag(data);
        }
 
       asn1_pop_tag(data);
       if (data->has_error) {
               asn1_free(data);
               return false;
       }
 
        *result = data_blob_talloc(mem_ctx, data->data, data->length);
        asn1_free(data);
        return true;
 }
 
 static const char *blob2string_talloc(TALLOC_CTX *mem_ctx,
	char *result = talloc_array(mem_ctx, char, blob.length+1);
	memcpy(result, blob.data, blob.length);
	result[blob.length] = '\0';
	return result;
}
