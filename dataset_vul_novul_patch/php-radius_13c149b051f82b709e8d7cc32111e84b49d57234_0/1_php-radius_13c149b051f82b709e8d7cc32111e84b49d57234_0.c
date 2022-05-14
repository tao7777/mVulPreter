rad_get_vendor_attr(u_int32_t *vendor, const void **data, size_t *len)
 {
 	struct vendor_attribute *attr;
 
	attr = (struct vendor_attribute *)*data;
 	*vendor = ntohl(attr->vendor_value);
 	*data = attr->attrib_data;
 	*len = attr->attrib_len - 2;
 
 	return (attr->attrib_type);
 }
