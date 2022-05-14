 INST_HANDLER (cpse) {	// CPSE Rd, Rr
 	int r = (buf[0] & 0xf) | ((buf[1] & 0x2) << 3);
 	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 0x1) << 4);
	RAnalOp next_op;
 
	avr_op_analyze (anal,
			&next_op,
			op->addr + op->size, buf + op->size, len - op->size,
			cpu);
	r_strbuf_fini (&next_op.esil);
	op->jump = op->addr + next_op.size + 2;

	op->cycles = 1;	// XXX: This is a bug, because depends on eval state,
	ESIL_A ("r%d,r%d,^,!,", r, d);			// Rr == Rd
	ESIL_A ("?{,%"PFMT64d",pc,=,},", op->jump);	// ?true => jmp
}
