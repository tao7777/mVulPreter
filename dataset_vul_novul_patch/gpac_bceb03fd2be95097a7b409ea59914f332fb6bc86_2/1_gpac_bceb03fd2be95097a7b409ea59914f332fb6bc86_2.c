static void oinf_entry_dump(GF_OperatingPointsInformation *ptr, FILE * trace)
{
	u32 i, count;

	if (!ptr) {
		fprintf(trace, "<OperatingPointsInformation scalability_mask=\"Multiview|Spatial scalability|Auxilary|unknown\" num_profile_tier_level=\"\" num_operating_points=\"\" dependency_layers=\"\">\n");

		fprintf(trace, " <ProfileTierLevel general_profile_space=\"\" general_tier_flag=\"\" general_profile_idc=\"\" general_profile_compatibility_flags=\"\" general_constraint_indicator_flags=\"\" />\n");

		fprintf(trace, "<OperatingPoint output_layer_set_idx=\"\" max_temporal_id=\"\" layer_count=\"\" minPicWidth=\"\" minPicHeight=\"\" maxPicWidth=\"\" maxPicHeight=\"\" maxChromaFormat=\"\" maxBitDepth=\"\" frame_rate_info_flag=\"\" bit_rate_info_flag=\"\" avgFrameRate=\"\" constantFrameRate=\"\" maxBitRate=\"\" avgBitRate=\"\"/>\n");

		fprintf(trace, "<Layer dependent_layerID=\"\" num_layers_dependent_on=\"\" dependent_on_layerID=\"\" dimension_identifier=\"\"/>\n");
		fprintf(trace, "</OperatingPointsInformation>\n");
		return;
	}


	fprintf(trace, "<OperatingPointsInformation");
	fprintf(trace, " scalability_mask=\"%u (", ptr->scalability_mask);
	switch (ptr->scalability_mask) {
	case 2:
		fprintf(trace, "Multiview");
		break;
	case 4:
		fprintf(trace, "Spatial scalability");
		break;
	case 8:
		fprintf(trace, "Auxilary");
		break;
	default:
		fprintf(trace, "unknown");
	}
	fprintf(trace, ")\" num_profile_tier_level=\"%u\"", gf_list_count(ptr->profile_tier_levels) );
	fprintf(trace, " num_operating_points=\"%u\" dependency_layers=\"%u\"", gf_list_count(ptr->operating_points), gf_list_count(ptr->dependency_layers));
	fprintf(trace, ">\n");


	count=gf_list_count(ptr->profile_tier_levels);
	for (i = 0; i < count; i++) {
		LHEVC_ProfileTierLevel *ptl = (LHEVC_ProfileTierLevel *)gf_list_get(ptr->profile_tier_levels, i);
		fprintf(trace, " <ProfileTierLevel general_profile_space=\"%u\" general_tier_flag=\"%u\" general_profile_idc=\"%u\" general_profile_compatibility_flags=\"%X\" general_constraint_indicator_flags=\""LLX"\" />\n", ptl->general_profile_space, ptl->general_tier_flag, ptl->general_profile_idc, ptl->general_profile_compatibility_flags, ptl->general_constraint_indicator_flags);
	}


	count=gf_list_count(ptr->operating_points);
	for (i = 0; i < count; i++) {
		LHEVC_OperatingPoint *op = (LHEVC_OperatingPoint *)gf_list_get(ptr->operating_points, i);
		fprintf(trace, "<OperatingPoint output_layer_set_idx=\"%u\"", op->output_layer_set_idx);
		fprintf(trace, " max_temporal_id=\"%u\" layer_count=\"%u\"", op->max_temporal_id, op->layer_count);
		fprintf(trace, " minPicWidth=\"%u\" minPicHeight=\"%u\"", op->minPicWidth, op->minPicHeight);
 		fprintf(trace, " maxPicWidth=\"%u\" maxPicHeight=\"%u\"", op->maxPicWidth, op->maxPicHeight);
 		fprintf(trace, " maxChromaFormat=\"%u\" maxBitDepth=\"%u\"", op->maxChromaFormat, op->maxBitDepth);
 		fprintf(trace, " frame_rate_info_flag=\"%u\" bit_rate_info_flag=\"%u\"", op->frame_rate_info_flag, op->bit_rate_info_flag);
		if (op->frame_rate_info_flag) 
 			fprintf(trace, " avgFrameRate=\"%u\" constantFrameRate=\"%u\"", op->avgFrameRate, op->constantFrameRate);
		if (op->bit_rate_info_flag) 
 			fprintf(trace, " maxBitRate=\"%u\" avgBitRate=\"%u\"", op->maxBitRate, op->avgBitRate);
 		fprintf(trace, "/>\n");
 	}
	count=gf_list_count(ptr->dependency_layers);
	for (i = 0; i < count; i++) {
		u32 j;
		LHEVC_DependentLayer *dep = (LHEVC_DependentLayer *)gf_list_get(ptr->dependency_layers, i);
		fprintf(trace, "<Layer dependent_layerID=\"%u\" num_layers_dependent_on=\"%u\"", dep->dependent_layerID, dep->num_layers_dependent_on);
		if (dep->num_layers_dependent_on) {
			fprintf(trace, " dependent_on_layerID=\"");
			for (j = 0; j < dep->num_layers_dependent_on; j++)
				fprintf(trace, "%d ", dep->dependent_on_layerID[j]);
			fprintf(trace, "\"");
		}
		fprintf(trace, " dimension_identifier=\"");
		for (j = 0; j < 16; j++)
			if (ptr->scalability_mask & (1 << j))
				fprintf(trace, "%d ", dep->dimension_identifier[j]);
		fprintf(trace, "\"/>\n");
	}
	fprintf(trace, "</OperatingPointsInformation>\n");
	return;
}
