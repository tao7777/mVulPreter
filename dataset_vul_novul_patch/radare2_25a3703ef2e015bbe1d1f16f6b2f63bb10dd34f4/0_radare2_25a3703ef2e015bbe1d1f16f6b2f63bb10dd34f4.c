INST_HANDLER (sbrx) {	// SBRC Rr, b
 	int b = buf[0] & 0x7;
 	int r = ((buf[0] >> 4) & 0xf) | ((buf[1] & 0x01) << 4);
	RAnalOp next_op = {0};
 
	avr_op_analyze (anal,
			&next_op,
			op->addr + op->size, buf + op->size, len - op->size,
			cpu);
	r_strbuf_fini (&next_op.esil);
	op->jump = op->addr + next_op.size + 2;

	op->cycles = 1;	// XXX: This is a bug, because depends on eval state,
	ESIL_A ("%d,1,<<,r%d,&,", b, r);			// Rr(b)
	ESIL_A ((buf[1] & 0xe) == 0xc
			? "!,"		// SBRC => branch if cleared
			: "!,!,");	// SBRS => branch if set
	ESIL_A ("?{,%"PFMT64d",pc,=,},", op->jump);	// ?true => jmp
}
