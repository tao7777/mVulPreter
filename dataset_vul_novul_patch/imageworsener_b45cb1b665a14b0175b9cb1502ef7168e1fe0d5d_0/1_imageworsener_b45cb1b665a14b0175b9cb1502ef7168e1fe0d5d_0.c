 static int get_exif_tag_int_value(struct iw_exif_state *e, unsigned int tag_pos,
	unsigned int *pv)
{
 	unsigned int field_type;
 	unsigned int value_count;
 
	field_type = iw_get_ui16_e(&e->d[tag_pos+2],e->endian);
	value_count = iw_get_ui32_e(&e->d[tag_pos+4],e->endian);
 
 	if(value_count!=1) return 0;
 
 	if(field_type==3) { // SHORT (uint16)
		*pv = iw_get_ui16_e(&e->d[tag_pos+8],e->endian);
 		return 1;
 	}
 	else if(field_type==4) { // LONG (uint32)
		*pv = iw_get_ui32_e(&e->d[tag_pos+8],e->endian);
 		return 1;
 	}
 
	return 0;
}
