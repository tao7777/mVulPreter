dissect_spoolss_uint16uni(tvbuff_t *tvb, int offset, packet_info *pinfo _U_,
			  proto_tree *tree, guint8 *drep _U_, char **data,
			  int hf_name)
{
	gint len, remaining;
	char *text;

	if (offset % 2)
		offset += 2 - (offset % 2);
 
 	/* Get remaining data in buffer as a string */
 
	remaining = tvb_captured_length_remaining(tvb, offset);
 	if (remaining <= 0) {
 		if (data)
 			*data = g_strdup("");
		return offset;
	}

	text = tvb_get_string_enc(NULL, tvb, offset, remaining, ENC_UTF_16|ENC_LITTLE_ENDIAN);
	len = (int)strlen(text);

	proto_tree_add_string(tree, hf_name, tvb, offset, len * 2, text);

	if (data)
		*data = text;
	else
		g_free(text);

	return offset + (len + 1) * 2;
}
