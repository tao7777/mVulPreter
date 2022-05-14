GF_Err gf_isom_oinf_read_entry(void *entry, GF_BitStream *bs)
{
	GF_OperatingPointsInformation* ptr = (GF_OperatingPointsInformation *)entry;
	u32 i, j, count;

	if (!ptr) return GF_BAD_PARAM;
	ptr->scalability_mask = gf_bs_read_u16(bs);
	gf_bs_read_int(bs, 2);//reserved
	count = gf_bs_read_int(bs, 6);
	for (i = 0; i < count; i++) {
		LHEVC_ProfileTierLevel *ptl;
		GF_SAFEALLOC(ptl, LHEVC_ProfileTierLevel);
		if (!ptl) return GF_OUT_OF_MEM;
		ptl->general_profile_space = gf_bs_read_int(bs, 2);
		ptl->general_tier_flag= gf_bs_read_int(bs, 1);
		ptl->general_profile_idc = gf_bs_read_int(bs, 5);
		ptl->general_profile_compatibility_flags = gf_bs_read_u32(bs);
		ptl->general_constraint_indicator_flags = gf_bs_read_long_int(bs, 48);
		ptl->general_level_idc = gf_bs_read_u8(bs);
		gf_list_add(ptr->profile_tier_levels, ptl);
	}
	count = gf_bs_read_u16(bs);
	for (i = 0; i < count; i++) {
		LHEVC_OperatingPoint *op;
		GF_SAFEALLOC(op, LHEVC_OperatingPoint);
		if (!op) return GF_OUT_OF_MEM;
 		op->output_layer_set_idx = gf_bs_read_u16(bs);
 		op->max_temporal_id = gf_bs_read_u8(bs);
 		op->layer_count = gf_bs_read_u8(bs);
		if (op->layer_count > ARRAY_LENGTH(op->layers_info))
			return GF_NON_COMPLIANT_BITSTREAM;
 		for (j = 0; j < op->layer_count; j++) {
 			op->layers_info[j].ptl_idx = gf_bs_read_u8(bs);
 			op->layers_info[j].layer_id = gf_bs_read_int(bs, 6);
			op->layers_info[j].is_outputlayer = gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE;
			op->layers_info[j].is_alternate_outputlayer = gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE;
		}
		op->minPicWidth = gf_bs_read_u16(bs);
		op->minPicHeight = gf_bs_read_u16(bs);
		op->maxPicWidth = gf_bs_read_u16(bs);
		op->maxPicHeight = gf_bs_read_u16(bs);
		op->maxChromaFormat = gf_bs_read_int(bs, 2);
		op->maxBitDepth = gf_bs_read_int(bs, 3) + 8;
		gf_bs_read_int(bs, 1);//reserved
		op->frame_rate_info_flag = gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE;
		op->bit_rate_info_flag = gf_bs_read_int(bs, 1) ? GF_TRUE : GF_FALSE;
		if (op->frame_rate_info_flag) {
			op->avgFrameRate = gf_bs_read_u16(bs);
			gf_bs_read_int(bs, 6); //reserved
			op->constantFrameRate = gf_bs_read_int(bs, 2);
		}
		if (op->bit_rate_info_flag) {
			op->maxBitRate = gf_bs_read_u32(bs);
			op->avgBitRate = gf_bs_read_u32(bs);
		}
		gf_list_add(ptr->operating_points, op);
	}
	count = gf_bs_read_u8(bs);
	for (i = 0; i < count; i++) {
		LHEVC_DependentLayer *dep;
		GF_SAFEALLOC(dep, LHEVC_DependentLayer);
		if (!dep) return GF_OUT_OF_MEM;
		dep->dependent_layerID = gf_bs_read_u8(bs);
		dep->num_layers_dependent_on = gf_bs_read_u8(bs);
		for (j = 0; j < dep->num_layers_dependent_on; j++)
			dep->dependent_on_layerID[j] = gf_bs_read_u8(bs);
		for (j = 0; j < 16; j++) {
			if (ptr->scalability_mask & (1 << j))
				dep->dimension_identifier[j] = gf_bs_read_u8(bs);
		}
		gf_list_add(ptr->dependency_layers, dep);
	}

	return GF_OK;
}
