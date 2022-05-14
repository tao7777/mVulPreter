R_API void r_anal_bb_free(RAnalBlock *bb) {
	if (!bb) {
		return;
	}
	r_anal_cond_free (bb->cond);
	R_FREE (bb->fingerprint);
	r_anal_diff_free (bb->diff);
	bb->diff = NULL;
	R_FREE (bb->op_bytes);
	r_anal_switch_op_free (bb->switch_op);
	bb->switch_op = NULL;
	bb->fingerprint = NULL;
	bb->cond = NULL;
	R_FREE (bb->label);
	R_FREE (bb->op_pos);
	R_FREE (bb->parent_reg_arena);
	if (bb->prev) {
		if (bb->prev->jumpbb == bb) {
			bb->prev->jumpbb = NULL;
		}
		if (bb->prev->failbb == bb) {
			bb->prev->failbb = NULL;
		}
		bb->prev = NULL;
	}
	if (bb->jumpbb) {
		bb->jumpbb->prev = NULL;
		bb->jumpbb = NULL;
	}
	if (bb->failbb) {
 		bb->failbb->prev = NULL;
 		bb->failbb = NULL;
 	}
	if (bb->next) {
		// avoid double free
		bb->next->prev = NULL;
	}
	R_FREE (bb); // double free
 }
