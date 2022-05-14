magiccheck(struct magic_set *ms, struct magic *m)
{
	uint64_t l = m->value.q;
	uint64_t v;
	float fl, fv;
	double dl, dv;
	int matched;
	union VALUETYPE *p = &ms->ms_value;

	switch (m->type) {
	case FILE_BYTE:
		v = p->b;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
		v = p->h;
		break;

	case FILE_LONG:
	case FILE_BELONG:
	case FILE_LELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		v = p->l;
		break;

	case FILE_QUAD:
	case FILE_LEQUAD:
	case FILE_BEQUAD:
	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
	case FILE_QWDATE:
	case FILE_BEQWDATE:
	case FILE_LEQWDATE:
		v = p->q;
		break;

	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		fl = m->value.f;
		fv = p->f;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = fv != fl;
			break;

		case '=':
			matched = fv == fl;
			break;

		case '>':
			matched = fv > fl;
			break;

		case '<':
			matched = fv < fl;
			break;

		default:
			file_magerror(ms, "cannot happen with float: invalid relation `%c'",
			    m->reln);
			return -1;
		}
		return matched;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		dl = m->value.d;
		dv = p->d;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = dv != dl;
			break;

		case '=':
			matched = dv == dl;
			break;

		case '>':
			matched = dv > dl;
			break;

		case '<':
			matched = dv < dl;
			break;

		default:
			file_magerror(ms, "cannot happen with double: invalid relation `%c'", m->reln);
			return -1;
		}
		return matched;

	case FILE_DEFAULT:
	case FILE_CLEAR:
		l = 0;
		v = 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
		l = 0;
		v = file_strncmp(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_BESTRING16:
	case FILE_LESTRING16:
		l = 0;
		v = file_strncmp16(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_SEARCH: { /* search ms->search.s for the string m->value.s */
		size_t slen;
		size_t idx;

		if (ms->search.s == NULL)
			return 0;

		slen = MIN(m->vallen, sizeof(m->value.s));
		l = 0;
		v = 0;

		for (idx = 0; m->str_range == 0 || idx < m->str_range; idx++) {
 			if (slen + idx > ms->search.s_len)
 				break;
 
			v = file_strncmp(m->value.s, ms->search.s + idx, slen,
			    m->str_flags);
 			if (v == 0) {	/* found match */
 				ms->search.offset += idx;
 				break;
			}
		}
		break;
	}
	case FILE_REGEX: {
		int rc;
		file_regex_t rx;

		if (ms->search.s == NULL)
			return 0;

		l = 0;
		rc = file_regcomp(&rx, m->value.s,
		    REG_EXTENDED|REG_NEWLINE|
		    ((m->str_flags & STRING_IGNORE_CASE) ? REG_ICASE : 0));
		if (rc) {
 			file_regerror(&rx, rc, ms);
 			v = (uint64_t)-1;
 		} else {
 			regmatch_t pmatch[1];
 			size_t slen = ms->search.s_len;
 #ifndef REG_STARTEND
 #define	REG_STARTEND	0
			char c;
 			if (slen != 0)
 				slen--;
 			c = ms->search.s[slen];
			((char *)(intptr_t)ms->search.s)[slen] = '\0';
#else
			pmatch[0].rm_so = 0;
			pmatch[0].rm_eo = slen;
#endif
			rc = file_regexec(&rx, (const char *)ms->search.s,
			    1, pmatch, REG_STARTEND);
#if REG_STARTEND == 0
			((char *)(intptr_t)ms->search.s)[l] = c;
#endif
			switch (rc) {
			case 0:
				ms->search.s += (int)pmatch[0].rm_so;
				ms->search.offset += (size_t)pmatch[0].rm_so;
				ms->search.rm_len =
				    (size_t)(pmatch[0].rm_eo - pmatch[0].rm_so);
				v = 0;
				break;

			case REG_NOMATCH:
				v = 1;
				break;

			default:
				file_regerror(&rx, rc, ms);
				v = (uint64_t)-1;
				break;
			}
		}
		file_regfree(&rx);
		if (v == (uint64_t)-1)
			return -1;
		break;
	}
	case FILE_INDIRECT:
	case FILE_USE:
	case FILE_NAME:
		return 1;
	default:
		file_magerror(ms, "invalid type %d in magiccheck()", m->type);
		return -1;
	}

	v = file_signextend(ms, m, v);

	switch (m->reln) {
	case 'x':
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT
			    "u == *any* = 1\n", (unsigned long long)v);
		matched = 1;
		break;

	case '!':
		matched = v != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u != %"
			    INT64_T_FORMAT "u = %d\n", (unsigned long long)v,
			    (unsigned long long)l, matched);
		break;

	case '=':
		matched = v == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u == %"
			    INT64_T_FORMAT "u = %d\n", (unsigned long long)v,
			    (unsigned long long)l, matched);
		break;

	case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u > %" INT64_T_FORMAT "u = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v > (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d > %" INT64_T_FORMAT "d = %d\n",
				    (long long)v, (long long)l, matched);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u < %" INT64_T_FORMAT "u = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v < (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d < %" INT64_T_FORMAT "d = %d\n",
				     (long long)v, (long long)l, matched);
		}
		break;

	case '&':
		matched = (v & l) == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) == %" INT64_T_FORMAT
			    "x) = %d\n", (unsigned long long)v,
			    (unsigned long long)l, (unsigned long long)l,
			    matched);
		break;

	case '^':
		matched = (v & l) != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) != %" INT64_T_FORMAT
			    "x) = %d\n", (unsigned long long)v,
			    (unsigned long long)l, (unsigned long long)l,
			    matched);
		break;

	default:
		file_magerror(ms, "cannot happen: invalid relation `%c'",
		    m->reln);
		return -1;
	}

	return matched;
}
