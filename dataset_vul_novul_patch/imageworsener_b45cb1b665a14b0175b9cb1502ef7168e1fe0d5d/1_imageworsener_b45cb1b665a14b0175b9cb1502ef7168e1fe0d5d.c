static int get_exif_tag_dbl_value(struct iw_exif_state *e, unsigned int tag_pos,
	double *pv)
{
	unsigned int field_type;
	unsigned int value_count;
 	unsigned int value_pos;
 	unsigned int numer, denom;
 
	field_type = iw_get_ui16_e(&e->d[tag_pos+2],e->endian);
	value_count = iw_get_ui32_e(&e->d[tag_pos+4],e->endian);
 
 	if(value_count!=1) return 0;
 
	if(field_type!=5) return 0; // 5=Rational (two uint32's)

 
	value_pos = iw_get_ui32_e(&e->d[tag_pos+8],e->endian);
 	if(value_pos > e->d_len-8) return 0;
 
	numer = iw_get_ui32_e(&e->d[value_pos  ],e->endian);
	denom = iw_get_ui32_e(&e->d[value_pos+4],e->endian);
 	if(denom==0) return 0;
 
 	*pv = ((double)numer)/denom;
	return 1;
}
