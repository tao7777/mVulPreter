rad_get_vendor_attr(u_int32_t *vendor, const void **data, size_t *len)
rad_get_vendor_attr(u_int32_t *vendor, unsigned char *type, const void **data, size_t *len, const void *raw, size_t raw_len)
 {
 	struct vendor_attribute *attr;
 
	if (raw_len < sizeof(struct vendor_attribute)) {
		return -1;
	}

	attr = (struct vendor_attribute *) raw;
 	*vendor = ntohl(attr->vendor_value);
	*type = attr->attrib_type;
 	*data = attr->attrib_data;
 	*len = attr->attrib_len - 2;
 
	if ((attr->attrib_len + 4) > raw_len) {
		return -1;
	}

 	return (attr->attrib_type);
 }
