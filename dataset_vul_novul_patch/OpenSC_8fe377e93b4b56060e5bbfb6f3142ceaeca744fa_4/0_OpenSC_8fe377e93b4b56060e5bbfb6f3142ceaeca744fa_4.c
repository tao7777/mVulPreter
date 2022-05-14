coolkey_find_attribute(sc_card_t *card, sc_cardctl_coolkey_attribute_t *attribute)
{
	u8 object_record_type;
	CK_ATTRIBUTE_TYPE attr_type = attribute->attribute_type;
	const u8 *obj = attribute->object->data;
	const u8 *attr = NULL;
	size_t buf_len = attribute->object->length;
	coolkey_object_header_t *object_head;
	int attribute_count,i;
	attribute->attribute_data_type = SC_CARDCTL_COOLKEY_ATTR_TYPE_STRING;
	attribute->attribute_length = 0;
	attribute->attribute_value = NULL;

	if (obj == NULL) {
		/* cast away const so we can cache the data value */
		int r = coolkey_fill_object(card, (sc_cardctl_coolkey_object_t *)attribute->object);
		if (r < 0) {
			return r;
		}
		obj = attribute->object->data;
	}

	/* should be a static assert so we catch this at compile time */
	assert(sizeof(coolkey_object_header_t) >= sizeof(coolkey_v0_object_header_t));
	/* make sure we have enough of the object to read the record_type */
	if (buf_len <= sizeof(coolkey_v0_object_header_t)) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	object_head = (coolkey_object_header_t *)obj;
	object_record_type = object_head->record_type;
	/* make sure it's a type we recognize */
	if ((object_record_type != COOLKEY_V1_OBJECT) && (object_record_type != COOLKEY_V0_OBJECT)) {
		return SC_ERROR_CORRUPTED_DATA;
	}


	/*
	 * now loop through all the attributes in the list. first find the start of the list
	 */
	attr = coolkey_attribute_start(obj, object_record_type, buf_len);
	if (attr == NULL) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	buf_len -= (attr-obj);

	/* now get the count */
	attribute_count = coolkey_get_attribute_count(obj, object_record_type, buf_len);
 	for (i=0; i < attribute_count; i++) {
 		size_t record_len = coolkey_get_attribute_record_len(attr, object_record_type, buf_len);
 		/* make sure we have the complete record */
		if (buf_len < record_len || record_len < 4) {
 				return SC_ERROR_CORRUPTED_DATA;
 		}
 		/* does the attribute match the one we are looking for */
		if (attr_type == coolkey_get_attribute_type(attr, object_record_type, record_len)) {
			/* yup, return it */
			return coolkey_get_attribute_data(attr, object_record_type, record_len, attribute);
		}
		/* go to the next attribute on the list */
		buf_len -= record_len;
		attr += record_len;
	}
	/* not find in attribute list, check the fixed attribute record */
	if (object_record_type == COOLKEY_V1_OBJECT) {
		unsigned long fixed_attributes = bebytes2ulong(object_head->fixed_attributes_values);

		return coolkey_get_attribute_data_fixed(attr_type, fixed_attributes, attribute);
	}
	return SC_ERROR_DATA_OBJECT_NOT_FOUND;
}
