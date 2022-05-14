GF_Err dinf_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e = gf_isom_box_array_read(s, bs, dinf_AddBox);
	if (e) {
 		return e;
 	}
 	if (!((GF_DataInformationBox *)s)->dref) {
		GF_Box* dref;
 		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing dref box in dinf\n"));
		dref = gf_isom_box_new(GF_ISOM_BOX_TYPE_DREF);
		((GF_DataInformationBox *)s)->dref = (GF_DataReferenceBox *)dref;
		gf_isom_box_add_for_dump_mode(s, dref);
 	}
 	return GF_OK;
 }
