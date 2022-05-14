 void HTML_put_string(HTStructured * me, const char *s)
 {
    HTChunk *target = NULL;

 #ifdef USE_PRETTYSRC
     char *translated_string = NULL;
 #endif

    if (s == NULL || (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT))
	return;
#ifdef USE_PRETTYSRC
    if (psrc_convert_string) {
	StrAllocCopy(translated_string, s);
	TRANSLATE_AND_UNESCAPE_ENTITIES(&translated_string, TRUE, FALSE);
	s = (const char *) translated_string;
    }
#endif

    switch (me->sp[0].tag_number) {

    case HTML_COMMENT:
 	break;			/* Do Nothing */
 
     case HTML_TITLE:
	target = &me->title;
 	break;
 
     case HTML_STYLE:
	target = &me->style_block;
 	break;
 
     case HTML_SCRIPT:
	target = &me->script;
 	break;
 
     case HTML_PRE:		/* Formatted text */
    case HTML_LISTING:		/* Literal text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
	/*
	 * We guarantee that the style is up-to-date in begin_litteral
	 */
	HText_appendText(me->text, s);
 	break;
 
     case HTML_OBJECT:
	target = &me->object;
 	break;
 
     case HTML_TEXTAREA:
	target = &me->textarea;
 	break;
 
     case HTML_SELECT:
     case HTML_OPTION:
	target = &me->option;
 	break;
 
     case HTML_MATH:
	target = &me->math;
 	break;
 
     default:			/* Free format text? */
	if (!me->sp->style->freeFormat) {
	    /*
	     * If we are within a preformatted text style not caught by the
	     * cases above (HTML_PRE or similar may not be the last element
	     * pushed on the style stack).  - kw
	     */
#ifdef USE_PRETTYSRC
	    if (psrc_view) {
		/*
		 * We do this so that a raw '\r' in the string will not be
		 * interpreted as an internal request to break a line - passing
		 * '\r' to HText_appendText is treated by it as a request to
		 * insert a blank line - VH
		 */
		for (; *s; ++s)
		    HTML_put_character(me, *s);
	    } else
#endif
		HText_appendText(me->text, s);
	    break;
	} else {
	    const char *p = s;
	    char c;

	    if (me->style_change) {
		for (; *p && ((*p == '\n') || (*p == '\r') ||
			      (*p == ' ') || (*p == '\t')); p++) ;	/* Ignore leaders */
		if (!*p)
		    break;
		UPDATE_STYLE;
	    }
	    for (; *p; p++) {
		if (*p == 13 && p[1] != 10) {
		    /*
		     * Treat any '\r' which is not followed by '\n' as '\n', to
		     * account for macintosh lineend in ALT attributes etc.  -
		     * kw
		     */
		    c = '\n';
		} else {
		    c = *p;
		}
		if (me->style_change) {
		    if ((c == '\n') || (c == ' ') || (c == '\t'))
			continue;	/* Ignore it */
		    UPDATE_STYLE;
		}
		if (c == '\n') {
		    if (!FIX_JAPANESE_SPACES) {
			if (me->in_word) {
			    if (HText_getLastChar(me->text) != ' ')
				HText_appendCharacter(me->text, ' ');
			    me->in_word = NO;
			}
		    }

		} else if (c == ' ' || c == '\t') {
		    if (HText_getLastChar(me->text) != ' ')
			HText_appendCharacter(me->text, ' ');

		} else if (c == '\r') {
		    /* ignore */
		} else {
		    HText_appendCharacter(me->text, c);
		    me->in_word = YES;
		}

		/* set the Last Character */
		if (c == '\n' || c == '\t') {
		    /* set it to a generic separator */
		    HText_setLastChar(me->text, ' ');
		} else if (c == '\r' &&
			   HText_getLastChar(me->text) == ' ') {
		    /*
		     * \r's are ignored.  In order to keep collapsing spaces
		     * correctly, we must default back to the previous
		     * separator, if there was one.  So we set LastChar to a
		     * generic separator.
		     */
		    HText_setLastChar(me->text, ' ');
		} else {
		    HText_setLastChar(me->text, c);
		}

 	    }			/* for */
 	}
     }				/* end switch */

    if (target != NULL) {
	if (target->data == s) {
	    CTRACE((tfp, "BUG: appending chunk to itself: `%.*s'\n",
		    target->size, target->data));
	} else {
	    HTChunkPuts(target, s);
	}
    }
 #ifdef USE_PRETTYSRC
     if (psrc_convert_string) {
 	psrc_convert_string = FALSE;
	FREE(translated_string);
    }
#endif
}
