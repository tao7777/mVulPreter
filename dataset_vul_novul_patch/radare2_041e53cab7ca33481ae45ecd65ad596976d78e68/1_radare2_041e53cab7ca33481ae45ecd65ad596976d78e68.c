 INST_HANDLER (lds) {	// LDS Rd, k
 	int d = ((buf[0] >> 4) & 0xf) | ((buf[1] & 0x1) << 4);
 	int k = (buf[3] << 8) | buf[2];
 	op->ptr = k;

	__generic_ld_st (op, "ram", 0, 1, 0, k, 0);
	ESIL_A ("r%d,=,", d);
}
