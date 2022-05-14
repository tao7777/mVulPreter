feed_table_block_tag(struct table *tbl,
		     char *line, struct table_mode *mode, int indent, int cmd)
{
    int offset;
    if (mode->indent_level <= 0 && indent == -1)
	return;
    if (mode->indent_level >= CHAR_MAX && indent == 1)
	return;
    setwidth(tbl, mode);
    feed_table_inline_tag(tbl, line, mode, -1);
    clearcontentssize(tbl, mode);
    if (indent == 1) {
	mode->indent_level++;
	if (mode->indent_level <= MAX_INDENT_LEVEL)
	    tbl->indent += INDENT_INCR;
    }
    else if (indent == -1) {
	mode->indent_level--;
 	if (mode->indent_level < MAX_INDENT_LEVEL)
 	    tbl->indent -= INDENT_INCR;
     }
    if (tbl->indent < 0)
	tbl->indent = 0;
     offset = tbl->indent;
     if (cmd == HTML_DT) {
 	if (mode->indent_level > 0 && mode->indent_level <= MAX_INDENT_LEVEL)
 	    offset -= INDENT_INCR;
	if (offset < 0)
	    offset = 0;
     }
     if (tbl->indent > 0) {
 	check_minimum0(tbl, 0);
	addcontentssize(tbl, offset);
    }
}
