GF_Err tenc_dump(GF_Box *a, FILE * trace)
{
	GF_TrackEncryptionBox *ptr = (GF_TrackEncryptionBox*) a;
	if (!a) return GF_BAD_PARAM;

	gf_isom_box_dump_start(a, "TrackEncryptionBox", trace);

	fprintf(trace, "isEncrypted=\"%d\"", ptr->isProtected);
	if (ptr->Per_Sample_IV_Size)
		fprintf(trace, " IV_size=\"%d\" KID=\"", ptr->Per_Sample_IV_Size);
	else {
		fprintf(trace, " constant_IV_size=\"%d\" constant_IV=\"", ptr->constant_IV_size);
		dump_data_hex(trace, (char *) ptr->constant_IV, ptr->constant_IV_size);
 		fprintf(trace, "\"  KID=\"");
 	}
 	dump_data_hex(trace, (char *) ptr->KID, 16);
	if (ptr->version) 
 		fprintf(trace, "\" crypt_byte_block=\"%d\" skip_byte_block=\"%d", ptr->crypt_byte_block, ptr->skip_byte_block);
 	fprintf(trace, "\">\n");
 	gf_isom_box_dump_done("TrackEncryptionBox", a, trace);
	return GF_OK;
}
