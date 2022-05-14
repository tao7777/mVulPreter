static void tm_reclaim_thread(struct thread_struct *thr,
			      struct thread_info *ti, uint8_t cause)
{
	unsigned long msr_diff = 0;

	/*
	 * If FP/VSX registers have been already saved to the
	 * thread_struct, move them to the transact_fp array.
	 * We clear the TIF_RESTORE_TM bit since after the reclaim
	 * the thread will no longer be transactional.
	 */
	if (test_ti_thread_flag(ti, TIF_RESTORE_TM)) {
		msr_diff = thr->ckpt_regs.msr & ~thr->regs->msr;
		if (msr_diff & MSR_FP)
			memcpy(&thr->transact_fp, &thr->fp_state,
			       sizeof(struct thread_fp_state));
		if (msr_diff & MSR_VEC)
			memcpy(&thr->transact_vr, &thr->vr_state,
			       sizeof(struct thread_vr_state));
		clear_ti_thread_flag(ti, TIF_RESTORE_TM);
 		msr_diff &= MSR_FP | MSR_VEC | MSR_VSX | MSR_FE0 | MSR_FE1;
 	}
 
 	tm_reclaim(thr, thr->regs->msr, cause);
 
 	/* Having done the reclaim, we now have the checkpointed
	 * FP/VSX values in the registers.  These might be valid
	 * even if we have previously called enable_kernel_fp() or
	 * flush_fp_to_thread(), so update thr->regs->msr to
	 * indicate their current validity.
	 */
	thr->regs->msr |= msr_diff;
}
