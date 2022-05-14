RCMS *r_pkcs7_parse_cms (const ut8 *buffer, ut32 length) {
	RASN1Object *object;
	RCMS *container;
	if (!buffer || !length) {
		return NULL;
	}
	container = R_NEW0 (RCMS);
	if (!container) {
 		return NULL;
 	}
 	object = r_asn1_create_object (buffer, length);
	if (!object || object->list.length != 2 || !object->list.objects ||
		!object->list.objects[0] || !object->list.objects[1] ||
		object->list.objects[1]->list.length != 1) {
 		r_asn1_free_object (object);
 		free (container);
 		return NULL;
	}
	container->contentType = r_asn1_stringify_oid (object->list.objects[0]->sector, object->list.objects[0]->length);
	r_pkcs7_parse_signeddata (&container->signedData, object->list.objects[1]->list.objects[0]);
	r_asn1_free_object (object);
	return container;
}
