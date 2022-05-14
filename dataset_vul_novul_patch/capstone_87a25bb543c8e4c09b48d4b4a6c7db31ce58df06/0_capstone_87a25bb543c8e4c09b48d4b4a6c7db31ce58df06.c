 x86_reg X86_insn_reg_intel(unsigned int id, enum cs_ac_type *access)
 {
	static bool intel_regs_sorted = false;
 	unsigned int first = 0;
 	unsigned int last = ARR_SIZE(insn_regs_intel) - 1;
	unsigned int mid;
 
 	if (!intel_regs_sorted) {
 		memcpy(insn_regs_intel_sorted, insn_regs_intel,
				sizeof(insn_regs_intel_sorted));
		qsort(insn_regs_intel_sorted,
				ARR_SIZE(insn_regs_intel_sorted),
				sizeof(struct insn_reg), regs_cmp);
 		intel_regs_sorted = true;
 	}
 
	if (insn_regs_intel_sorted[0].insn > id ||
			insn_regs_intel_sorted[last].insn < id) {
		return 0;
	}

 	while (first <= last) {
		mid = (first + last) / 2;
 		if (insn_regs_intel_sorted[mid].insn < id) {
 			first = mid + 1;
 		} else if (insn_regs_intel_sorted[mid].insn == id) {
			if (access) {
				*access = insn_regs_intel_sorted[mid].access;
			}
			return insn_regs_intel_sorted[mid].reg;
		} else {
			if (mid == 0)
 				break;
 			last = mid - 1;
 		}
 	}
 
	return 0;
}
