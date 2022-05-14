static int java_switch_op(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *data, int len) {
	ut8 op_byte = data[0];
	ut64 offset = addr - java_get_method_start ();
	ut8 pos = (offset+1)%4 ? 1 + 4 - (offset+1)%4 : 1;
 
 	if (op_byte == 0xaa) {
		if (pos + 8 + 8 > len) {
 			return op->size;
 		}
		const int min_val = (ut32)(UINT (data, pos + 4));
		const int max_val = (ut32)(UINT (data, pos + 8));
 
 		ut32 default_loc = (ut32) (UINT (data, pos)), cur_case = 0;
 		op->switch_op = r_anal_switch_op_new (addr, min_val, default_loc);
		RAnalCaseOp *caseop = NULL;
		pos += 12;
		if (max_val > min_val && ((max_val - min_val)<(UT16_MAX/4))) {
			for (cur_case = 0; cur_case <= max_val - min_val; pos += 4, cur_case++) {
				if (pos + 4 >= len) {
					break;
				}
				int offset = (int)(ut32)(R_BIN_JAVA_UINT (data, pos));
				caseop = r_anal_switch_op_add_case (op->switch_op,
					addr + pos, cur_case + min_val, addr + offset);
				if (caseop) {
					caseop->bb_ref_to = addr+offset;
					caseop->bb_ref_from = addr; // TODO figure this one out
				}
			}
		} else {
			eprintf ("Invalid switch boundaries at 0x%"PFMT64x"\n", addr);
		}
	}
	op->size = pos;
	return op->size;
}
