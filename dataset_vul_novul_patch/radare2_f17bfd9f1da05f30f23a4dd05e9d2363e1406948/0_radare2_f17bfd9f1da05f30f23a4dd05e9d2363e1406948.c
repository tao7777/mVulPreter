static int opmov(RAsm *a, ut8 *data, const Opcode *op) {
	int l = 0;
	st64 offset = 0;
	int mod = 0;
	int base = 0;
	int rex = 0;
	ut64 immediate = 0;
	if (op->operands[1].type & OT_CONSTANT) {
		if (!op->operands[1].is_good_flag) {
			return -1;
		}
		if (op->operands[1].immediate == -1) {
			return -1;
		}
		immediate = op->operands[1].immediate * op->operands[1].sign;
		if (op->operands[0].type & OT_GPREG && !(op->operands[0].type & OT_MEMORY)) {
			if (a->bits == 64 && ((op->operands[0].type & OT_QWORD) | (op->operands[1].type & OT_QWORD))) {
				if (!(op->operands[1].type & OT_CONSTANT) && op->operands[1].extended) {
					data[l++] = 0x49;
				} else {
					data[l++] = 0x48;
				}
			} else if (op->operands[0].extended) {
				data[l++] = 0x41;
			}
			if (op->operands[0].type & OT_WORD) {
				if (a->bits > 16) {
					data[l++] = 0x66;
				}
			}
			if (op->operands[0].type & OT_BYTE) {
				data[l++] = 0xb0 | op->operands[0].reg;
				data[l++] = immediate;
			} else {
				if (a->bits == 64 &&
					((op->operands[0].type & OT_QWORD) |
					(op->operands[1].type & OT_QWORD)) &&
					immediate < UT32_MAX) {
						data[l++] = 0xc7;
				 		data[l++] = 0xc0 | op->operands[0].reg;
				} else {
					data[l++] = 0xb8 | op->operands[0].reg;
				}
				data[l++] = immediate;
				data[l++] = immediate >> 8;
				if (!(op->operands[0].type & OT_WORD)) {
					data[l++] = immediate >> 16;
					data[l++] = immediate >> 24;
				}
				if (a->bits == 64 && immediate > UT32_MAX) {
					data[l++] = immediate >> 32;
					data[l++] = immediate >> 40;
					data[l++] = immediate >> 48;
					data[l++] = immediate >> 56;
				}
			}
		} else if (op->operands[0].type & OT_MEMORY) {
			if (!op->operands[0].explicit_size) {
				if (op->operands[0].type & OT_GPREG) {
					((Opcode *)op)->operands[0].dest_size = op->operands[0].reg_size;
				} else {
					return -1;
				}
			}

			int dest_bits = 8 * ((op->operands[0].dest_size & ALL_SIZE) >> OPSIZE_SHIFT);
			int reg_bits = 8 * ((op->operands[0].reg_size & ALL_SIZE) >> OPSIZE_SHIFT);
			int offset = op->operands[0].offset * op->operands[0].offset_sign;

			bool use_aso = false;
			if (reg_bits < a->bits) {
				use_aso = true;
			}

			bool use_oso = false;
			if (dest_bits == 16) {
				use_oso = true;
			}

			bool rip_rel = op->operands[0].regs[0] == X86R_RIP;

			int rex = 1 << 6;
			bool use_rex = false;
			if (dest_bits == 64) {			//W field
				use_rex = true;
				rex |= 1 << 3;
			}
			if (op->operands[0].extended) {		//B field
				use_rex = true;
				rex |= 1;
			}

			int opcode;
			if (dest_bits == 8) {
				opcode = 0xc6;
			} else {
				opcode = 0xc7;
			}

			int modrm = 0;
			int mod;
			int reg = 0;
			int rm;
			bool use_sib = false;
			int sib;
			if (offset == 0) {
				mod = 0;
			} else if (offset < 128 && offset > -129) {
				mod = 1;
			} else {
				mod = 2;
			}

			if (reg_bits == 16) {
				if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == X86R_SI) {
					rm = B0000;
				} else if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == X86R_DI) {
					rm = B0001;
				} else if (op->operands[0].regs[0] == X86R_BP && op->operands[0].regs[1] == X86R_SI) {
					rm = B0010;
				} else if (op->operands[0].regs[0] == X86R_BP && op->operands[0].regs[1] == X86R_DI) {
					rm = B0011;
				} else if (op->operands[0].regs[0] == X86R_SI && op->operands[0].regs[1] == -1) {
					rm = B0100;
				} else if (op->operands[0].regs[0] == X86R_DI && op->operands[0].regs[1] == -1) {
					rm = B0101;
				} else if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == -1) {
					rm = B0111;
				} else {
					return -1;
				}
				modrm = (mod << 6) | (reg << 3) | rm;
			} else {
				if (op->operands[0].extended) {
					rm = op->operands[0].reg;
				} else {
					rm = op->operands[0].regs[0];
				}
				if (rm == 5 && mod == 0) {
					mod = 1;
				}

				int index = op->operands[0].regs[1];
				int scale = getsib(op->operands[0].scale[1]);
				if (index != -1) {
					use_sib = true;
					sib = (scale << 6) | (index << 3) | rm;
				} else if (rm == 4) {
					use_sib = true;
					sib = 0x24;
				}
				if (use_sib) {
					rm = B0100;
				}
				if (rip_rel) {
					modrm = (B0000 << 6) | (reg << 3) | B0101;
					sib = (scale << 6) | (B0100 << 3) | B0101;
				} else {
					modrm = (mod << 6) | (reg << 3) | rm;
				}
			}

			if (use_aso) {
				data[l++] = 0x67;
			}
			if (use_oso) {
				data[l++] = 0x66;
			}
			if (use_rex) {
				data[l++] = rex;
			}
			data[l++] = opcode;
			data[l++] = modrm;
			if (use_sib) {
				data[l++] = sib;
			}
			if (mod == 1) {
				data[l++] = offset;
			} else if (reg_bits == 16 && mod == 2) {
				data[l++] = offset;
				data[l++] = offset >> 8;
			} else if (mod == 2 || rip_rel) {
				data[l++] = offset;
				data[l++] = offset >> 8;
				data[l++] = offset >> 16;
				data[l++] = offset >> 24;
			}
			int byte;
			for (byte = 0; byte < dest_bits && byte < 32; byte += 8) {
				data[l++] = (immediate >> byte);
			}
		}
	} else if (op->operands[1].type & OT_REGALL &&
			 !(op->operands[1].type & OT_MEMORY)) {
		if (op->operands[0].type & OT_CONSTANT) {
			return -1;
		}
		if (op->operands[0].type & OT_REGTYPE & OT_SEGMENTREG &&
		    op->operands[1].type & OT_REGTYPE & OT_SEGMENTREG) {
				return -1;
		}
		if (op->operands[0].type & OT_REGTYPE && op->operands[1].type & OT_REGTYPE) {
			if (!((op->operands[0].type & ALL_SIZE) &
			(op->operands[1].type & ALL_SIZE))) {
				return -1;
			}
		}

		if (a->bits == 64) {
			if (op->operands[0].extended) {
				rex = 1;
			}
			if (op->operands[1].extended) {
				rex += 4;
			}
			if (op->operands[1].type & OT_QWORD) {
				if (!(op->operands[0].type & OT_QWORD)) {
					data[l++] = 0x67;
					data[l++] = 0x48;
				}
			}
			if (op->operands[1].type & OT_QWORD &&
				op->operands[0].type & OT_QWORD) {
				data[l++] = 0x48 | rex;
			}
			if (op->operands[1].type & OT_DWORD &&
				op->operands[0].type & OT_DWORD) {
				data[l++] = 0x40 | rex;
			}
		} else if (op->operands[0].extended && op->operands[1].extended) {
			data[l++] = 0x45;
		}
		offset = op->operands[0].offset * op->operands[0].offset_sign;
		if (op->operands[1].type & OT_REGTYPE & OT_SEGMENTREG) {
			data[l++] = 0x8c;
		} else {
			if (op->operands[0].type & OT_WORD) {
				data[l++] = 0x66;
			}
			data[l++] = (op->operands[0].type & OT_BYTE) ? 0x88 : 0x89;
		}

		if (op->operands[0].scale[0] > 1) {
				data[l++] = op->operands[1].reg << 3 | 4;
				data[l++] = getsib (op->operands[0].scale[0]) << 6 |
						    op->operands[0].regs[0] << 3 | 5;

				data[l++] = offset;
				data[l++] = offset >> 8;
				data[l++] = offset >> 16;
				data[l++] = offset >> 24;

				return l;
			}

		if (!(op->operands[0].type & OT_MEMORY)) {
			if (op->operands[0].reg == X86R_UNDEFINED ||
				op->operands[1].reg == X86R_UNDEFINED) {
				return -1;
			}
			mod = 0x3;
			data[l++] = mod << 6 | op->operands[1].reg << 3 | op->operands[0].reg;
		} else if (op->operands[0].regs[0] == X86R_UNDEFINED) {
			data[l++] = op->operands[1].reg << 3 | 0x5;
			data[l++] = offset;
			data[l++] = offset >> 8;
			data[l++] = offset >> 16;
			data[l++] = offset >> 24;
		} else {
			if (op->operands[0].type & OT_MEMORY) {
				if (op->operands[0].regs[1] != X86R_UNDEFINED) {
					data[l++] = op->operands[1].reg << 3 | 0x4;
					data[l++] = op->operands[0].regs[1] << 3 | op->operands[0].regs[0];
					return l;
				}
				if (offset) {
					mod = (offset > 128 || offset < -129) ? 0x2 : 0x1;
				}
				if (op->operands[0].regs[0] == X86R_EBP) {
					mod = 0x2;
				}
				data[l++] = mod << 6 | op->operands[1].reg << 3 | op->operands[0].regs[0];
				if (op->operands[0].regs[0] == X86R_ESP) {
					data[l++] = 0x24;
				}
				if (offset) {
					data[l++] = offset;
				}
				if (mod == 2) {
					data[l++] = offset >> 8;
					data[l++] = offset >> 16;
					data[l++] = offset >> 24;
				}
			}
		}
	} else if (op->operands[1].type & OT_MEMORY) {
		if (op->operands[0].type & OT_MEMORY) {
			return -1;
		}
		offset = op->operands[1].offset * op->operands[1].offset_sign;
		if (op->operands[0].reg == X86R_EAX && op->operands[1].regs[0] == X86R_UNDEFINED) {
			if (a->bits == 64) {
				data[l++] = 0x48;
			}
			if (op->operands[0].type & OT_BYTE) {
				data[l++] = 0xa0;
			} else {
				data[l++] = 0xa1;
			}
			data[l++] = offset;
			data[l++] = offset >> 8;
			data[l++] = offset >> 16;
			data[l++] = offset >> 24;
			if (a->bits == 64) {
				data[l++] = offset >> 32;
				data[l++] = offset >> 40;
				data[l++] = offset >> 48;
				data[l++] = offset >> 54;
			}
			return l;
		}
		if (op->operands[0].type & OT_BYTE && a->bits == 64 && op->operands[1].regs[0]) {
			if (op->operands[1].regs[0] >= X86R_R8 &&
			    op->operands[0].reg < 4) {
				data[l++] = 0x41;
				data[l++] = 0x8a;
				data[l++] = op->operands[0].reg << 3 | (op->operands[1].regs[0] - 8);
				return l;
			}
			return -1;
		}

		if (op->operands[1].type & OT_REGTYPE & OT_SEGMENTREG) {
 			if (op->operands[1].scale[0] == 0) {
 				return -1;
 			}
			data[l++] = SEG_REG_PREFIXES[op->operands[1].regs[0] % 6];
 			data[l++] = 0x8b;
			data[l++] = (((ut32)op->operands[0].reg) << 3) | 0x5;
 			data[l++] = offset;
 			data[l++] = offset >> 8;
 			data[l++] = offset >> 16;
			data[l++] = offset >> 24;
			return l;
		}

		if (a->bits == 64) {
			if (op->operands[0].type & OT_QWORD) {
				if (!(op->operands[1].type & OT_QWORD)) {
					if (op->operands[1].regs[0] != -1) {
						data[l++] = 0x67;
					}
					data[l++] = 0x48;
				}
			} else if (op->operands[1].type & OT_DWORD) {
				data[l++] = 0x44;
			} else if (!(op->operands[1].type & OT_QWORD)) {
				data[l++] = 0x67;
			}
			if (op->operands[1].type & OT_QWORD &&
				op->operands[0].type & OT_QWORD) {
				data[l++] = 0x48;
			}
		}

		if (op->operands[0].type & OT_WORD) {
			data[l++] = 0x66;
			data[l++] = op->operands[1].type & OT_BYTE ? 0x8a : 0x8b;
		} else {
			data[l++] = (op->operands[1].type & OT_BYTE ||
				op->operands[0].type & OT_BYTE) ?
				0x8a : 0x8b;
		}

		if (op->operands[1].regs[0] == X86R_UNDEFINED) {
			if (a->bits == 64) {
				data[l++] = op->operands[0].reg << 3 | 0x4;
				data[l++] = 0x25;
			} else {
				data[l++] = op->operands[0].reg << 3 | 0x5;
			}
			data[l++] = offset;
			data[l++] = offset >> 8;
			data[l++] = offset >> 16;
			data[l++] = offset >> 24;
		} else {
			if (op->operands[1].scale[0] > 1) {
				data[l++] = op->operands[0].reg << 3 | 4;

				if (op->operands[1].scale[0] >= 2) {
					base = 5;
				}
				if (base) {
					data[l++] = getsib (op->operands[1].scale[0]) << 6 | op->operands[1].regs[0] << 3 | base;
				} else {
					data[l++] = getsib (op->operands[1].scale[0]) << 3 | op->operands[1].regs[0];
				}
				if (offset || base) {
					data[l++] = offset;
					data[l++] = offset >> 8;
					data[l++] = offset >> 16;
					data[l++] = offset >> 24;
				}
				return l;
			}
			if (op->operands[1].regs[1] != X86R_UNDEFINED) {
				data[l++] = op->operands[0].reg << 3 | 0x4;
				data[l++] = op->operands[1].regs[1] << 3 | op->operands[1].regs[0];
				return l;
			}

			if (offset || op->operands[1].regs[0] == X86R_EBP) {
				mod = 0x2;
				if (op->operands[1].offset > 127) {
					mod = 0x4;
				}
			}
			if (a->bits == 64 && offset && op->operands[0].type & OT_QWORD) {
				if (op->operands[1].regs[0] == X86R_RIP) {
					data[l++] = 0x5;
				} else {
					if (op->operands[1].offset > 127) {
						data[l++] = 0x80 | op->operands[0].reg << 3 | op->operands[1].regs[0];
					} else {
						data[l++] = 0x40 | op->operands[1].regs[0];
					}
				}
				if (op->operands[1].offset > 127) {
					mod = 0x1;
				}
			} else {
				if (op->operands[1].regs[0] == X86R_EIP && (op->operands[0].type & OT_DWORD)) {
					data[l++] = 0x0d;
				} else if (op->operands[1].regs[0] == X86R_RIP && (op->operands[0].type & OT_QWORD)) {
					data[l++] = 0x05;
				} else {
					data[l++] = mod << 5 | op->operands[0].reg << 3 | op->operands[1].regs[0];
				}
			}
			if (op->operands[1].regs[0] == X86R_ESP) {
				data[l++] = 0x24;
			}
			if (mod >= 0x2) {
				data[l++] = offset;
				if (op->operands[1].offset > 128 || op->operands[1].regs[0] == X86R_EIP) {
					data[l++] = offset >> 8;
					data[l++] = offset >> 16;
					data[l++] = offset >> 24;
				}
			} else if (a->bits == 64 && (offset || op->operands[1].regs[0] == X86R_RIP)) {
				data[l++] = offset;
				if (op->operands[1].offset > 127 || op->operands[1].regs[0] == X86R_RIP) {
					data[l++] = offset >> 8;
					data[l++] = offset >> 16;
					data[l++] = offset >> 24;
				}
			}
		}
	}
	return l;
}
