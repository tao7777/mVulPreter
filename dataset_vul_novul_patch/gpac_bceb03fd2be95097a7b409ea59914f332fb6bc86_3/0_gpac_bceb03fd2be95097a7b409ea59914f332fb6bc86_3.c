GF_Err sgpd_dump(GF_Box *a, FILE * trace)
{
	u32 i;
	GF_SampleGroupDescriptionBox *ptr = (GF_SampleGroupDescriptionBox*) a;
	if (!a) return GF_BAD_PARAM;

	gf_isom_box_dump_start(a, "SampleGroupDescriptionBox", trace);

	if (ptr->grouping_type)
		fprintf(trace, "grouping_type=\"%s\"", gf_4cc_to_str(ptr->grouping_type) );
	if (ptr->version==1) fprintf(trace, " default_length=\"%d\"", ptr->default_length);
	if ((ptr->version>=2) && ptr->default_description_index) fprintf(trace, " default_group_index=\"%d\"", ptr->default_description_index);
	fprintf(trace, ">\n");
	for (i=0; i<gf_list_count(ptr->group_descriptions); i++) {
		void *entry = gf_list_get(ptr->group_descriptions, i);
		switch (ptr->grouping_type) {
		case GF_ISOM_SAMPLE_GROUP_ROLL:
			fprintf(trace, "<RollRecoveryEntry roll_distance=\"%d\" />\n", ((GF_RollRecoveryEntry*)entry)->roll_distance );
			break;
		case GF_ISOM_SAMPLE_GROUP_PROL:
			fprintf(trace, "<AudioPreRollEntry roll_distance=\"%d\" />\n", ((GF_RollRecoveryEntry*)entry)->roll_distance );
			break;
		case GF_ISOM_SAMPLE_GROUP_TELE:
			fprintf(trace, "<TemporalLevelEntry level_independently_decodable=\"%d\"/>\n", ((GF_TemporalLevelEntry*)entry)->level_independently_decodable);
			break;
		case GF_ISOM_SAMPLE_GROUP_RAP:
			fprintf(trace, "<VisualRandomAccessEntry num_leading_samples_known=\"%s\"", ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples_known ? "yes" : "no");
			if (((GF_VisualRandomAccessEntry*)entry)->num_leading_samples_known)
				fprintf(trace, " num_leading_samples=\"%d\"", ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples);
			fprintf(trace, "/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_SYNC:
			fprintf(trace, "<SyncSampleGroupEntry NAL_unit_type=\"%d\"/>\n", ((GF_SYNCEntry*)entry)->NALU_type);
			break;
		case GF_ISOM_SAMPLE_GROUP_SEIG:
			fprintf(trace, "<CENCSampleEncryptionGroupEntry IsEncrypted=\"%d\" IV_size=\"%d\" KID=\"", ((GF_CENCSampleEncryptionGroupEntry*)entry)->IsProtected, ((GF_CENCSampleEncryptionGroupEntry*)entry)->Per_Sample_IV_size);
			dump_data_hex(trace, (char *)((GF_CENCSampleEncryptionGroupEntry*)entry)->KID, 16);
			if ((((GF_CENCSampleEncryptionGroupEntry*)entry)->IsProtected == 1) && !((GF_CENCSampleEncryptionGroupEntry*)entry)->Per_Sample_IV_size) {
				fprintf(trace, "\" constant_IV_size=\"%d\"  constant_IV=\"", ((GF_CENCSampleEncryptionGroupEntry*)entry)->constant_IV_size);
				dump_data_hex(trace, (char *)((GF_CENCSampleEncryptionGroupEntry*)entry)->constant_IV, ((GF_CENCSampleEncryptionGroupEntry*)entry)->constant_IV_size);
			}
			fprintf(trace, "\"/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_OINF:
			oinf_entry_dump(entry, trace);
			break;
		case GF_ISOM_SAMPLE_GROUP_LINF:
			linf_dump(entry, trace);
			break;
 		case GF_ISOM_SAMPLE_GROUP_TRIF:
 			trif_dump(trace, (char *) ((GF_DefaultSampleGroupDescriptionEntry*)entry)->data,  ((GF_DefaultSampleGroupDescriptionEntry*)entry)->length);
 			break;

 		case GF_ISOM_SAMPLE_GROUP_NALM:
 			nalm_dump(trace, (char *) ((GF_DefaultSampleGroupDescriptionEntry*)entry)->data,  ((GF_DefaultSampleGroupDescriptionEntry*)entry)->length);
 			break;
		case GF_ISOM_SAMPLE_GROUP_SAP:
			fprintf(trace, "<SAPEntry dependent_flag=\"%d\" SAP_type=\"%d\" />\n", ((GF_SAPEntry*)entry)->dependent_flag, ((GF_SAPEntry*)entry)->SAP_type);
			break;
		default:
			fprintf(trace, "<DefaultSampleGroupDescriptionEntry size=\"%d\" data=\"", ((GF_DefaultSampleGroupDescriptionEntry*)entry)->length);
			dump_data(trace, (char *) ((GF_DefaultSampleGroupDescriptionEntry*)entry)->data,  ((GF_DefaultSampleGroupDescriptionEntry*)entry)->length);
			fprintf(trace, "\"/>\n");
		}
	}
	if (!ptr->size) {
		switch (ptr->grouping_type) {
		case GF_ISOM_SAMPLE_GROUP_ROLL:
			fprintf(trace, "<RollRecoveryEntry roll_distance=\"\"/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_PROL:
			fprintf(trace, "<AudioPreRollEntry roll_distance=\"\"/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_TELE:
			fprintf(trace, "<TemporalLevelEntry level_independently_decodable=\"\"/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_RAP:
			fprintf(trace, "<VisualRandomAccessEntry num_leading_samples_known=\"yes|no\" num_leading_samples=\"\" />\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_SYNC:
			fprintf(trace, "<SyncSampleGroupEntry NAL_unit_type=\"\" />\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_SEIG:
			fprintf(trace, "<CENCSampleEncryptionGroupEntry IsEncrypted=\"\" IV_size=\"\" KID=\"\" constant_IV_size=\"\"  constant_IV=\"\"/>\n");
			break;
		case GF_ISOM_SAMPLE_GROUP_OINF:
			oinf_entry_dump(NULL, trace);
			break;
		case GF_ISOM_SAMPLE_GROUP_LINF:
			linf_dump(NULL, trace);
			break;
		case GF_ISOM_SAMPLE_GROUP_TRIF:
			trif_dump(trace, NULL, 0);
			break;
		case GF_ISOM_SAMPLE_GROUP_NALM:
			nalm_dump(trace, NULL, 0);
			break;
		case GF_ISOM_SAMPLE_GROUP_SAP:
			fprintf(trace, "<SAPEntry dependent_flag=\"\" SAP_type=\"\" />\n");
			break;
		default:
			fprintf(trace, "<DefaultSampleGroupDescriptionEntry size=\"\" data=\"\"/>\n");
		}
	}

	gf_isom_box_dump_done("SampleGroupDescriptionBox", a, trace);
	return GF_OK;
}
