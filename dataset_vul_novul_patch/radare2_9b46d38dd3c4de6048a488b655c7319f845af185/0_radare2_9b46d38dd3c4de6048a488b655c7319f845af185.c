static int parseOperand(RAsm *a, const char *str, Operand *op, bool isrepop) {
	size_t pos, nextpos = 0;
	x86newTokenType last_type;
	int size_token = 1;
	bool explicit_size = false;
	int reg_index = 0;
	op->type = 0;
	while (size_token) {
		pos = nextpos;
		last_type = getToken (str, &pos, &nextpos);

		if (!r_str_ncasecmp (str + pos, "ptr", 3)) {
			continue;
		} else if (!r_str_ncasecmp (str + pos, "byte", 4)) {
			op->type |= OT_MEMORY | OT_BYTE;
			op->dest_size = OT_BYTE;
			explicit_size = true;
		} else if (!r_str_ncasecmp (str + pos, "word", 4)) {
			op->type |= OT_MEMORY | OT_WORD;
			op->dest_size = OT_WORD;
			explicit_size = true;
		} else if (!r_str_ncasecmp (str + pos, "dword", 5)) {
			op->type |= OT_MEMORY | OT_DWORD;
			op->dest_size = OT_DWORD;
			explicit_size = true;
		} else if (!r_str_ncasecmp (str + pos, "qword", 5)) {
			op->type |= OT_MEMORY | OT_QWORD;
			op->dest_size = OT_QWORD;
			explicit_size = true;
		} else if (!r_str_ncasecmp (str + pos, "oword", 5)) {
			op->type |= OT_MEMORY | OT_OWORD;
			op->dest_size = OT_OWORD;
			explicit_size = true;
		} else if (!r_str_ncasecmp (str + pos, "tbyte", 5)) {
			op->type |= OT_MEMORY | OT_TBYTE;
			op->dest_size = OT_TBYTE;
			explicit_size = true;
		} else { // the current token doesn't denote a size
			size_token = 0;
		}
	}

	if (str[pos] == '[') {
		if (!op->type) {
			op->type = OT_MEMORY;
		}
		op->offset = op->scale[0] = op->scale[1] = 0;

		ut64 temp = 1;
		Register reg = X86R_UNDEFINED;
		bool first_reg = true;
		while (str[pos] != ']') {
			if (pos > nextpos) {
				break;
			}
			pos = nextpos;
			if (!str[pos]) {
				break;
			}
			last_type = getToken (str, &pos, &nextpos);

 			if (last_type == TT_SPECIAL) {
 				if (str[pos] == '+' || str[pos] == '-' || str[pos] == ']') {
 					if (reg != X86R_UNDEFINED) {
						if (reg_index < 2) {
							op->regs[reg_index] = reg;
							op->scale[reg_index] = temp;
						}
 						++reg_index;
 					} else {
 						op->offset += temp;
						if (reg_index < 2) {
							op->regs[reg_index] = X86R_UNDEFINED;
						}
 					}
 					temp = 1;
 					reg = X86R_UNDEFINED;
 				} else if (str[pos] == '*') {

				}
			}
			else if (last_type == TT_WORD) {
				ut32 reg_type = 0;

				if (reg != X86R_UNDEFINED) {
					op->type = 0;	// Make the result invalid
				}

				nextpos = pos;
				reg = parseReg (a, str, &nextpos, &reg_type);

				if (first_reg) {
					op->extended = false;
					if (reg > 8) {
						op->extended = true;
						op->reg = reg - 9;
					}
					first_reg = false;
				} else if (reg > 8) {
					op->reg = reg - 9;
				}
				if (reg_type & OT_REGTYPE & OT_SEGMENTREG) {
					op->reg = reg;
					op->type = reg_type;
					parse_segment_offset (a, str, &nextpos, op, reg_index);
					return nextpos;
				}

				if (!explicit_size) {
					op->type |= reg_type;
				}
				op->reg_size = reg_type;
				op->explicit_size = explicit_size;

				if (!(reg_type & OT_GPREG)) {
					op->type = 0;	// Make the result invalid
				}
			}
			else {
				char *p = strchr (str, '+');
				op->offset_sign = 1;
				if (!p) {
					p = strchr (str, '-');
					if (p) {
						op->offset_sign = -1;
					}
				}
				char * plus = strchr (str, '+');
				char * minus = strchr (str, '-');
				char * closeB = strchr (str, ']');
				if (plus && minus && plus < closeB && minus < closeB) {
					op->offset_sign = -1;
				}
				char *tmp;
				tmp = malloc (strlen (str + pos) + 1);
				strcpy (tmp, str + pos);
				strtok (tmp, "+-");
				st64 read = getnum (a, tmp);
				free (tmp);
				temp *= read;
			}
		}
	} else if (last_type == TT_WORD) {   // register
		nextpos = pos;
		RFlagItem *flag;

		if (isrepop) {
			op->is_good_flag = false;
			strncpy (op->rep_op, str, MAX_REPOP_LENGTH - 1);
			op->rep_op[MAX_REPOP_LENGTH - 1] = '\0';
			return nextpos;
		}

		op->reg = parseReg (a, str, &nextpos, &op->type);

		op->extended = false;
		if (op->reg > 8) {
			op->extended = true;
			op->reg -= 9;
		}
		if (op->type & OT_REGTYPE & OT_SEGMENTREG) {
			parse_segment_offset (a, str, &nextpos, op, reg_index);
			return nextpos;
		}
		if (op->reg == X86R_UNDEFINED) {
			op->is_good_flag = false;
			if (a->num && a->num->value == 0) {
				return nextpos;
			}
			op->type = OT_CONSTANT;
			RCore *core = a->num? (RCore *)(a->num->userptr): NULL;
			if (core && (flag = r_flag_get (core->flags, str))) {
				op->is_good_flag = true;
			}

			char *p = strchr (str, '-');
			if (p) {
				op->sign = -1;
				str = ++p;
			}
			op->immediate = getnum (a, str);
		} else if (op->reg < X86R_UNDEFINED) {
			strncpy (op->rep_op, str, MAX_REPOP_LENGTH - 1);
			op->rep_op[MAX_REPOP_LENGTH - 1] = '\0';
		}
	} else {                             // immediate
		op->type = OT_CONSTANT;
		op->sign = 1;
		char *p = strchr (str, '-');
		if (p) {
			op->sign = -1;
			str = ++p;
		}
		op->immediate = getnum (a, str);
	}

	return nextpos;
}
