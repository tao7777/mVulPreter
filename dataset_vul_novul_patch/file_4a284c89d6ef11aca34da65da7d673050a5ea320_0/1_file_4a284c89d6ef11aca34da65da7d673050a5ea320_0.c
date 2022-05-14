string_modifier_check(struct magic_set *ms, struct magic *m)
{
 	if ((ms->flags & MAGIC_CHECK) == 0)
 		return 0;
 
	if (m->type != FILE_PSTRING && (m->str_flags & PSTRING_LEN) != 0) {
 		file_magwarn(ms,
 		    "'/BHhLl' modifiers are only allowed for pascal strings\n");
 		return -1;
	}
	switch (m->type) {
	case FILE_BESTRING16:
	case FILE_LESTRING16:
		if (m->str_flags != 0) {
			file_magwarn(ms,
			    "no modifiers allowed for 16-bit strings\n");
			return -1;
		}
		break;
	case FILE_STRING:
	case FILE_PSTRING:
		if ((m->str_flags & REGEX_OFFSET_START) != 0) {
			file_magwarn(ms,
			    "'/%c' only allowed on regex and search\n",
			    CHAR_REGEX_OFFSET_START);
			return -1;
		}
		break;
	case FILE_SEARCH:
		if (m->str_range == 0) {
			file_magwarn(ms,
			    "missing range; defaulting to %d\n",
                            STRING_DEFAULT_RANGE);
			m->str_range = STRING_DEFAULT_RANGE;
			return -1;
		}
		break;
	case FILE_REGEX:
		if ((m->str_flags & STRING_COMPACT_WHITESPACE) != 0) {
			file_magwarn(ms, "'/%c' not allowed on regex\n",
			    CHAR_COMPACT_WHITESPACE);
			return -1;
		}
		if ((m->str_flags & STRING_COMPACT_OPTIONAL_WHITESPACE) != 0) {
			file_magwarn(ms, "'/%c' not allowed on regex\n",
			    CHAR_COMPACT_OPTIONAL_WHITESPACE);
			return -1;
		}
		break;
	default:
		file_magwarn(ms, "coding error: m->type=%d\n",
		    m->type);
		return -1;
	}
	return 0;
}
