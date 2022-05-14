static int _6502_op(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *data, int len) {
	char addrbuf[64];
	const int buffsize = sizeof (addrbuf) - 1;

	memset (op, '\0', sizeof (RAnalOp));
	op->size = snes_op_get_size (1, 1, &snes_op[data[0]]);	//snes-arch is similiar to nes/6502
	op->addr = addr;
	op->type = R_ANAL_OP_TYPE_UNK;
	op->id = data[0];
	r_strbuf_init (&op->esil);
	switch (data[0]) {
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x07:
	case 0x0b:
	case 0x0c:
	case 0x0f:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x17:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1f:
	case 0x22:
	case 0x23:
	case 0x27:
	case 0x2b:
	case 0x2f:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x37:
	case 0x3a:
	case 0x3b:
	case 0x3c:
	case 0x3f:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x47:
	case 0x4b:
	case 0x4f:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x57:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5f:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x67:
	case 0x6b:
	case 0x6f:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x77:
	case 0x7a:
	case 0x7b:
	case 0x7c:
	case 0x7f:
	case 0x80:
	case 0x82:
	case 0x83:
	case 0x87:
	case 0x89:
	case 0x8b:
	case 0x8f:
	case 0x92:
	case 0x93:
	case 0x97:
	case 0x9b:
	case 0x9c:
	case 0x9e:
	case 0x9f:
	case 0xa3:
	case 0xa7:
	case 0xab:
	case 0xaf:
	case 0xb2:
	case 0xb3:
	case 0xb7:
	case 0xbb:
	case 0xbf:
	case 0xc2:
	case 0xc3:
	case 0xc7:
	case 0xcb:
	case 0xcf:
	case 0xd2:
	case 0xd3:
	case 0xd4:
	case 0xd7:
	case 0xda:
	case 0xdb:
	case 0xdc:
	case 0xdf:
	case 0xe2:
	case 0xe3:
	case 0xe7:
	case 0xeb:
	case 0xef:
	case 0xf2:
	case 0xf3:
	case 0xf4:
	case 0xf7:
	case 0xfa:
	case 0xfb:
	case 0xfc:
	case 0xff:
		op->size = 1;
		op->type = R_ANAL_OP_TYPE_ILL;
		break;

	case 0x00: // brk
		op->cycles = 7;
		op->type = R_ANAL_OP_TYPE_SWI;
		op->size = 1;
		r_strbuf_set (&op->esil, ",1,I,=,0,D,=,flags,0x10,|,0x100,sp,+,=[1],pc,1,+,0xfe,sp,+,=[2],3,sp,-=,0xfffe,[2],pc,=");
		break;

	case 0x78: // sei
	case 0x58: // cli
	case 0x38: // sec
	case 0x18: // clc
	case 0xf8: // sed
	case 0xd8: // cld
	case 0xb8: // clv
		op->cycles = 2;
		op->type = R_ANAL_OP_TYPE_NOP;
		_6502_anal_esil_flags (op, data[0]);
		break;
	case 0x24: // bit $ff
	case 0x2c: // bit $ffff
		op->type = R_ANAL_OP_TYPE_MOV;
		_6502_anal_esil_get_addr_pattern3 (op, data, addrbuf, buffsize, 0);
		r_strbuf_setf (&op->esil, "a,%s,[1],&,0x80,&,!,!,N,=,a,%s,[1],&,0x40,&,!,!,V,=,a,%s,[1],&,0xff,&,!,Z,=",addrbuf, addrbuf, addrbuf);
		break;
	case 0x69: // adc #$ff
	case 0x65: // adc $ff
	case 0x75: // adc $ff,x
	case 0x6d: // adc $ffff
	case 0x7d: // adc $ffff,x
	case 0x79: // adc $ffff,y
	case 0x61: // adc ($ff,x)
	case 0x71: // adc ($ff,y)
		op->type = R_ANAL_OP_TYPE_ADD;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0x69) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,+=,C,NUM,$c7,C,=,a,+=,$c7,C,|=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,+=,C,NUM,$c7,C,=,a,+=,$c7,C,|=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		r_strbuf_append (&op->esil, ",a,a,=,$z,Z,=");
		break;
	case 0xe9: // sbc #$ff
	case 0xe5: // sbc $ff
	case 0xf5: // sbc $ff,x
	case 0xed: // sbc $ffff
	case 0xfd: // sbc $ffff,x
	case 0xf9: // sbc $ffff,y
	case 0xe1: // sbc ($ff,x)
	case 0xf1: // sbc ($ff,y)
		op->type = R_ANAL_OP_TYPE_SUB;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0xe9) // immediate mode
			r_strbuf_setf (&op->esil, "C,!,%s,+,a,-=", addrbuf);
		else	r_strbuf_setf (&op->esil, "C,!,%s,[1],+,a,-=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_BNZ);
		r_strbuf_append (&op->esil, ",a,a,=,$z,Z,=,C,!=");
		break;
	case 0x09: // ora #$ff
	case 0x05: // ora $ff
	case 0x15: // ora $ff,x
	case 0x0d: // ora $ffff
	case 0x1d: // ora $ffff,x
	case 0x19: // ora $ffff,y
	case 0x01: // ora ($ff,x)
	case 0x11: // ora ($ff),y
		op->type = R_ANAL_OP_TYPE_OR;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0x09) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,|=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,|=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x29: // and #$ff
	case 0x25: // and $ff
	case 0x35: // and $ff,x
	case 0x2d: // and $ffff
	case 0x3d: // and $ffff,x
	case 0x39: // and $ffff,y
	case 0x21: // and ($ff,x)
	case 0x31: // and ($ff),y
		op->type = R_ANAL_OP_TYPE_AND;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0x29) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,&=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,&=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x49: // eor #$ff
	case 0x45: // eor $ff
	case 0x55: // eor $ff,x
	case 0x4d: // eor $ffff
	case 0x5d: // eor $ffff,x
	case 0x59: // eor $ffff,y
	case 0x41: // eor ($ff,x)
	case 0x51: // eor ($ff),y
		op->type = R_ANAL_OP_TYPE_XOR;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0x49) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,^=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,^=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x0a: // asl a
	case 0x06: // asl $ff
	case 0x16: // asl $ff,x
	case 0x0e: // asl $ffff
	case 0x1e: // asl $ffff,x
		op->type = R_ANAL_OP_TYPE_SHL;
		if (data[0] == 0x0a) {
			r_strbuf_set (&op->esil, "1,a,<<=,$c7,C,=,a,a,=");
		} else  {
			_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
			r_strbuf_setf (&op->esil, "1,%s,[1],<<,%s,=[1],$c7,C,=", addrbuf, addrbuf);
		}
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x4a: // lsr a
	case 0x46: // lsr $ff
	case 0x56: // lsr $ff,x
	case 0x4e: // lsr $ffff
	case 0x5e: // lsr $ffff,x
		op->type = R_ANAL_OP_TYPE_SHR;
		if (data[0] == 0x4a) {
			r_strbuf_set (&op->esil, "1,a,&,C,=,1,a,>>=");
		} else {
			_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
			r_strbuf_setf (&op->esil, "1,%s,[1],&,C,=,1,%s,[1],>>,%s,=[1]", addrbuf, addrbuf, addrbuf);
		}
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x2a: // rol a
	case 0x26: // rol $ff
	case 0x36: // rol $ff,x
	case 0x2e: // rol $ffff
	case 0x3e: // rol $ffff,x
		op->type = R_ANAL_OP_TYPE_ROL;
		if (data[0] == 0x2a) {
			r_strbuf_set (&op->esil, "1,a,<<,C,|,a,=,$c7,C,=,a,a,=");
		} else {
			_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
			r_strbuf_setf (&op->esil, "1,%s,[1],<<,C,|,%s,=[1],$c7,C,=", addrbuf, addrbuf);
		}
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x6a: // ror a
	case 0x66: // ror $ff
	case 0x76: // ror $ff,x
	case 0x6e: // ror $ffff
	case 0x7e: // ror $ffff,x
		op->type = R_ANAL_OP_TYPE_ROR;
		if (data[0] == 0x6a) {
			r_strbuf_set (&op->esil, "C,N,=,1,a,&,C,=,1,a,>>,7,N,<<,|,a,=");
		} else {
			_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
			r_strbuf_setf (&op->esil, "C,N,=,1,%s,[1],&,C,=,1,%s,[1],>>,7,N,<<,|,%s,=[1]", addrbuf, addrbuf, addrbuf);
		}
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0xe6: // inc $ff
	case 0xf6: // inc $ff,x
	case 0xee: // inc $ffff
	case 0xfe: // inc $ffff,x
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
		r_strbuf_setf (&op->esil, "%s,++=[1]", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0xc6: // dec $ff
	case 0xd6: // dec $ff,x
	case 0xce: // dec $ffff
	case 0xde: // dec $ffff,x
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'x');
		r_strbuf_setf (&op->esil, "%s,--=[1]", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0xe8: // inx
	case 0xc8: // iny
		op->cycles = 2;
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_inc_reg (op, data[0], "+");
		break;
	case 0xca: // dex
	case 0x88: // dey
		op->cycles = 2;
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_inc_reg (op, data[0], "-");
		break;
	case 0xc9: // cmp #$ff
	case 0xc5: // cmp $ff
	case 0xd5: // cmp $ff,x
	case 0xcd: // cmp $ffff
	case 0xdd: // cmp $ffff,x
	case 0xd9: // cmp $ffff,y
	case 0xc1: // cmp ($ff,x)
	case 0xd1: // cmp ($ff),y
		op->type = R_ANAL_OP_TYPE_CMP;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0xc9) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,==", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,==", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_BNZ);
		r_strbuf_append (&op->esil, ",C,!,C,=");
		break;
	case 0xe0: // cpx #$ff
	case 0xe4: // cpx $ff
	case 0xec: // cpx $ffff
		op->type = R_ANAL_OP_TYPE_CMP;
		_6502_anal_esil_get_addr_pattern3 (op, data, addrbuf, buffsize, 0);
		if (data[0] == 0xe0) // immediate mode
			r_strbuf_setf (&op->esil, "%s,x,==", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],x,==", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_BNZ);
		r_strbuf_append (&op->esil, ",C,!,C,=");
		break;
	case 0xc0: // cpy #$ff
	case 0xc4: // cpy $ff
	case 0xcc: // cpy $ffff
		op->type = R_ANAL_OP_TYPE_CMP;
		_6502_anal_esil_get_addr_pattern3 (op, data, addrbuf, buffsize, 0);
		if (data[0] == 0xc0) // immediate mode
			r_strbuf_setf (&op->esil, "%s,y,==", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],y,==", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_BNZ);
		r_strbuf_append (&op->esil, ",C,!,C,=");
		break;
	case 0x10: // bpl $ffff
	case 0x30: // bmi $ffff
	case 0x50: // bvc $ffff
	case 0x70: // bvs $ffff
	case 0x90: // bcc $ffff
	case 0xb0: // bcs $ffff
	case 0xd0: // bne $ffff
	case 0xf0: // beq $ffff
 		op->cycles = 2;
 		op->failcycles = 3;
 		op->type = R_ANAL_OP_TYPE_CJMP;
		if (len > 1) {
			if (data[1] <= 127) {
				op->jump = addr + data[1] + op->size;
			} else {
				op->jump = addr - (256 - data[1]) + op->size;
			}
		} else {
			op->jump = addr;
		}
 		op->fail = addr + op->size;
		_6502_anal_esil_ccall (op, data[0]);
		break;
	case 0x20: // jsr $ffff
		op->cycles = 6;
		op->type = R_ANAL_OP_TYPE_CALL;
		op->jump = data[1] | data[2] << 8;
		op->stackop = R_ANAL_STACK_INC;
		op->stackptr = 2;
		r_strbuf_setf (&op->esil, "1,pc,-,0xff,sp,+,=[2],0x%04x,pc,=,2,sp,-=", op->jump);
		break;
	case 0x4c: // jmp $ffff
		op->cycles = 3;
		op->type = R_ANAL_OP_TYPE_JMP;
		op->jump = data[1] | data[2] << 8;
		r_strbuf_setf (&op->esil, "0x%04x,pc,=", op->jump);
		break;
	case 0x6c: // jmp ($ffff)
		op->cycles = 5;
		op->type = R_ANAL_OP_TYPE_UJMP;
		r_strbuf_setf (&op->esil, "0x%04x,[2],pc,=", data[1] | data[2] << 8);
		break;
	case 0x60: // rts
		op->eob = true;
		op->type = R_ANAL_OP_TYPE_RET;
		op->cycles = 6;
		op->stackop = R_ANAL_STACK_INC;
		op->stackptr = -2;
		r_strbuf_set (&op->esil, "0x101,sp,+,[2],pc,=,pc,++=,2,sp,+=");
		break;
	case 0x40: // rti
		op->eob = true;
		op->type = R_ANAL_OP_TYPE_RET;
		op->cycles = 6;
		op->stackop = R_ANAL_STACK_INC;
		op->stackptr = -3;
		r_strbuf_set (&op->esil, "0x101,sp,+,[1],flags,=,0x102,sp,+,[2],pc,=,3,sp,+=");
		break;
	case 0xea: // nop
		op->type = R_ANAL_OP_TYPE_NOP;
		op->cycles = 2;
		break;
	case 0xa9: // lda #$ff
	case 0xa5: // lda $ff
	case 0xb5: // lda $ff,x
	case 0xad: // lda $ffff
	case 0xbd: // lda $ffff,x
	case 0xb9: // lda $ffff,y
	case 0xa1: // lda ($ff,x)
	case 0xb1: // lda ($ff),y
		op->type = R_ANAL_OP_TYPE_LOAD;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		if (data[0] == 0xa9) // immediate mode
			r_strbuf_setf (&op->esil, "%s,a,=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],a,=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0xa2: // ldx #$ff
	case 0xa6: // ldx $ff
	case 0xb6: // ldx $ff,y
	case 0xae: // ldx $ffff
	case 0xbe: // ldx $ffff,y
		op->type = R_ANAL_OP_TYPE_LOAD;
		_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'y');
		if (data[0] == 0xa2) // immediate mode
			r_strbuf_setf (&op->esil, "%s,x,=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],x,=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0xa0: // ldy #$ff
	case 0xa4: // ldy $ff
	case 0xb4: // ldy $ff,x
	case 0xac: // ldy $ffff
	case 0xbc: // ldy $ffff,x
		op->type = R_ANAL_OP_TYPE_LOAD;
		_6502_anal_esil_get_addr_pattern3 (op, data, addrbuf, buffsize, 'x');
		if (data[0] == 0xa0) // immediate mode
			r_strbuf_setf (&op->esil, "%s,y,=", addrbuf);
		else	r_strbuf_setf (&op->esil, "%s,[1],y,=", addrbuf);
		_6502_anal_update_flags (op, _6502_FLAGS_NZ);
		break;
	case 0x85: // sta $ff
	case 0x95: // sta $ff,x
	case 0x8d: // sta $ffff
	case 0x9d: // sta $ffff,x
	case 0x99: // sta $ffff,y
	case 0x81: // sta ($ff,x)
	case 0x91: // sta ($ff),y
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_get_addr_pattern1 (op, data, addrbuf, buffsize);
		r_strbuf_setf (&op->esil, "a,%s,=[1]", addrbuf);
		break;
	case 0x86: // stx $ff
	case 0x96: // stx $ff,y
	case 0x8e: // stx $ffff
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_get_addr_pattern2 (op, data, addrbuf, buffsize, 'y');
		r_strbuf_setf (&op->esil, "x,%s,=[1]", addrbuf);
		break;
	case 0x84: // sty $ff
	case 0x94: // sty $ff,x
	case 0x8c: // sty $ffff
		op->type = R_ANAL_OP_TYPE_STORE;
		_6502_anal_esil_get_addr_pattern3 (op, data, addrbuf, buffsize, 'x');
		r_strbuf_setf (&op->esil, "y,%s,=[1]", addrbuf);
		break;
	case 0x08: // php
	case 0x48: // pha
		op->type = R_ANAL_OP_TYPE_PUSH;
		op->cycles = 3;
		op->stackop = R_ANAL_STACK_INC;
		op->stackptr = 1;
		_6502_anal_esil_push (op, data[0]);
		break;
	case 0x28: // plp
	case 0x68: // plp
		op->type = R_ANAL_OP_TYPE_POP;
		op->cycles = 4;
		op->stackop = R_ANAL_STACK_INC;
		op->stackptr = -1;
		_6502_anal_esil_pop (op, data[0]);
		break;
	case 0xaa: // tax
	case 0x8a: // txa
	case 0xa8: // tay
	case 0x98: // tya
		op->type = R_ANAL_OP_TYPE_MOV;
		op->cycles = 2;
		_6502_anal_esil_mov (op, data[0]);
		break;
	case 0x9a: // txs
		op->type = R_ANAL_OP_TYPE_MOV;
		op->cycles = 2;
		op->stackop = R_ANAL_STACK_SET;
		_6502_anal_esil_mov (op, data[0]);
		break;
	case 0xba: // tsx
		op->type = R_ANAL_OP_TYPE_MOV;
		op->cycles = 2;
		op->stackop = R_ANAL_STACK_GET;
		_6502_anal_esil_mov (op, data[0]);
		break;
	}
	return op->size;
}
