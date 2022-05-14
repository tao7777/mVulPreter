 static OPCODE_DESC* avr_op_analyze(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *buf, int len, CPU_MODEL *cpu) {
 	OPCODE_DESC *opcode_desc;
 	ut16 ins = (buf[1] << 8) | buf[0];
 	int fail;
 	char *t;

	memset (op, 0, sizeof (RAnalOp));
	op->ptr = UT64_MAX;
	op->val = UT64_MAX;
	op->jump = UT64_MAX;
	r_strbuf_init (&op->esil);

	for (opcode_desc = opcodes; opcode_desc->handler; opcode_desc++) {
		if ((ins & opcode_desc->mask) == opcode_desc->selector) {
			fail = 0;

			op->cycles = opcode_desc->cycles;
			op->size = opcode_desc->size;
			op->type = opcode_desc->type;
			op->jump = UT64_MAX;
			op->fail = UT64_MAX;
			op->addr = addr;

			r_strbuf_setf (&op->esil, "");

			opcode_desc->handler (anal, op, buf, len, &fail, cpu);
			if (fail) {
				goto INVALID_OP;
			}
			if (op->cycles <= 0) {
				opcode_desc->cycles = 2;
			}
			op->nopcode = (op->type == R_ANAL_OP_TYPE_UNK);

			t = r_strbuf_get (&op->esil);
			if (t && strlen (t) > 1) {
				t += strlen (t) - 1;
				if (*t == ',') {
					*t = '\0';
				}
			}

			return opcode_desc;
		}
	}

	if ((ins & 0xff00) == 0xff00 && (ins & 0xf) > 7) {
		goto INVALID_OP;
	}

INVALID_OP:
	op->family = R_ANAL_OP_FAMILY_UNKNOWN;
	op->type = R_ANAL_OP_TYPE_UNK;
	op->addr = addr;
	op->fail = UT64_MAX;
	op->jump = UT64_MAX;
	op->ptr = UT64_MAX;
	op->val = UT64_MAX;
	op->nopcode = 1;
	op->cycles = 1;
	op->size = 2;
	r_strbuf_set (&op->esil, "1,$");

	return NULL;
}
