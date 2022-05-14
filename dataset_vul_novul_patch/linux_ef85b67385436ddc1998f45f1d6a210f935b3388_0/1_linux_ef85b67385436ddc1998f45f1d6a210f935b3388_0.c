static inline bool is_exception(u32 intr_info)
 {
 	return (intr_info & (INTR_INFO_INTR_TYPE_MASK | INTR_INFO_VALID_MASK))
		== (INTR_TYPE_HARD_EXCEPTION | INTR_INFO_VALID_MASK);
 }
