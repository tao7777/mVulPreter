mprint(struct magic_set *ms, struct magic *m)
{
	uint64_t v;
	float vf;
	double vd;
	int64_t t = 0;
 	char buf[128], tbuf[26];
	union VALUETYPE *p = &ms->ms_value;

  	switch (m->type) {
  	case FILE_BYTE:
		v = file_signextend(ms, m, (uint64_t)p->b);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%d",
			    (unsigned char)v);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%d"),
			    (unsigned char) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(char);
		break;

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		v = file_signextend(ms, m, (uint64_t)p->h);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%u",
			    (unsigned short)v);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%u"),
			    (unsigned short) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(short);
		break;

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		v = file_signextend(ms, m, (uint64_t)p->l);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%u", (uint32_t) v);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%u"), (uint32_t) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(int32_t);
  		break;

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		v = file_signextend(ms, m, p->q);
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%" INT64_T_FORMAT "u",
			    (unsigned long long)v);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%" INT64_T_FORMAT "u"),
			    (unsigned long long) v) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(int64_t);
  		break;

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!') {
			if (file_printf(ms, F(ms, m, "%s"), m->value.s) == -1)
				return -1;
			t = ms->offset + m->vallen;
		}
		else {
			char *str = p->s;

			/* compute t before we mangle the string? */
			t = ms->offset + strlen(str);

			if (*m->value.s == '\0')
				str[strcspn(str, "\n")] = '\0';

			if (m->str_flags & STRING_TRIM) {
				char *last;
				while (isspace((unsigned char)*str))
					str++;
				last = str;
				while (*last)
					last++;
				--last;
				while (isspace((unsigned char)*last))
					last--;
				*++last = '\0';
			}

			if (file_printf(ms, F(ms, m, "%s"), str) == -1)
				return -1;

			if (m->type == FILE_PSTRING)
				t += file_pstring_length_size(m);
		}
		break;

	case FILE_DATE:
	case FILE_BEDATE:
 	case FILE_LEDATE:
 	case FILE_MEDATE:
 		if (file_printf(ms, F(ms, m, "%s"),
		    file_fmttime(p->l + m->num_mask, FILE_T_LOCAL, tbuf)) == -1)
 			return -1;
 		t = ms->offset + sizeof(uint32_t);
 		break;

	case FILE_LDATE:
	case FILE_BELDATE:
 	case FILE_LELDATE:
 	case FILE_MELDATE:
 		if (file_printf(ms, F(ms, m, "%s"),
		    file_fmttime(p->l + m->num_mask, 0, tbuf)) == -1)
 			return -1;
 		t = ms->offset + sizeof(uint32_t);
 		break;

	case FILE_QDATE:
 	case FILE_BEQDATE:
 	case FILE_LEQDATE:
 		if (file_printf(ms, F(ms, m, "%s"),
		    file_fmttime(p->q + m->num_mask, FILE_T_LOCAL, tbuf)) == -1)
 			return -1;
 		t = ms->offset + sizeof(uint64_t);
 		break;

	case FILE_QLDATE:
 	case FILE_BEQLDATE:
 	case FILE_LEQLDATE:
 		if (file_printf(ms, F(ms, m, "%s"),
		    file_fmttime(p->q + m->num_mask, 0, tbuf)) == -1)
 			return -1;
 		t = ms->offset + sizeof(uint64_t);
 		break;

	case FILE_QWDATE:
 	case FILE_BEQWDATE:
 	case FILE_LEQWDATE:
 		if (file_printf(ms, F(ms, m, "%s"),
		    file_fmttime(p->q + m->num_mask, FILE_T_WINDOWS, tbuf)) == -1)
 			return -1;
 		t = ms->offset + sizeof(uint64_t);
 		break;

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		vf = p->f;
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vf);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%g"), vf) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(float);
  		break;

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		vd = p->d;
		switch (check_fmt(ms, m)) {
		case -1:
			return -1;
		case 1:
			(void)snprintf(buf, sizeof(buf), "%g", vd);
			if (file_printf(ms, F(ms, m, "%s"), buf) == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, F(ms, m, "%g"), vd) == -1)
				return -1;
			break;
		}
		t = ms->offset + sizeof(double);
  		break;

	case FILE_REGEX: {
		char *cp;
		int rval;

		cp = strndup((const char *)ms->search.s, ms->search.rm_len);
		if (cp == NULL) {
			file_oomem(ms, ms->search.rm_len);
			return -1;
		}
		rval = file_printf(ms, F(ms, m, "%s"), cp);
		free(cp);

		if (rval == -1)
			return -1;

		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + ms->search.rm_len;
		break;
	}

	case FILE_SEARCH:
	  	if (file_printf(ms, F(ms, m, "%s"), m->value.s) == -1)
			return -1;
		if ((m->str_flags & REGEX_OFFSET_START))
			t = ms->search.offset;
		else
			t = ms->search.offset + m->vallen;
		break;

	case FILE_DEFAULT:
	case FILE_CLEAR:
	  	if (file_printf(ms, "%s", m->desc) == -1)
			return -1;
		t = ms->offset;
		break;

	case FILE_INDIRECT:
	case FILE_USE:
	case FILE_NAME:
		t = ms->offset;
		break;

	default:
		file_magerror(ms, "invalid m->type (%d) in mprint()", m->type);
		return -1;
	}
	return (int32_t)t;
}
