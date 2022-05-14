formUpdateBuffer(Anchor *a, Buffer *buf, FormItemList *form)
{
    Buffer save;
    char *p;
    int spos, epos, rows, c_rows, pos, col = 0;
    Line *l;

    copyBuffer(&save, buf);
    gotoLine(buf, a->start.line);
    switch (form->type) {
    case FORM_TEXTAREA:
    case FORM_INPUT_TEXT:
    case FORM_INPUT_FILE:
    case FORM_INPUT_PASSWORD:
    case FORM_INPUT_CHECKBOX:
    case FORM_INPUT_RADIO:
#ifdef MENU_SELECT
    case FORM_SELECT:
#endif				/* MENU_SELECT */
	spos = a->start.pos;
	epos = a->end.pos;
	break;
    default:
	spos = a->start.pos + 1;
	epos = a->end.pos - 1;
    }
    switch (form->type) {
    case FORM_INPUT_CHECKBOX:
    case FORM_INPUT_RADIO:
	if (buf->currentLine == NULL ||
	    spos >= buf->currentLine->len || spos < 0)
	    break;
	if (form->checked)
	    buf->currentLine->lineBuf[spos] = '*';
	else
	    buf->currentLine->lineBuf[spos] = ' ';
	break;
    case FORM_INPUT_TEXT:
    case FORM_INPUT_FILE:
    case FORM_INPUT_PASSWORD:
    case FORM_TEXTAREA:
#ifdef MENU_SELECT
    case FORM_SELECT:
	if (form->type == FORM_SELECT) {
	    p = form->label->ptr;
	    updateSelectOption(form, form->select_option);
	}
	else
#endif				/* MENU_SELECT */
	{
	    if (!form->value)
		break;
	    p = form->value->ptr;
	}
	l = buf->currentLine;
	if (!l)
	    break;
	if (form->type == FORM_TEXTAREA) {
	    int n = a->y - buf->currentLine->linenumber;
	    if (n > 0)
		for (; l && n; l = l->prev, n--) ;
	    else if (n < 0)
		for (; l && n; l = l->prev, n++) ;
	    if (!l)
		break;
	}
 	rows = form->rows ? form->rows : 1;
 	col = COLPOS(l, a->start.pos);
 	for (c_rows = 0; c_rows < rows; c_rows++, l = l->next) {
	    if (l == NULL)
		break;
 	    if (rows > 1) {
 		pos = columnPos(l, col);
 		a = retrieveAnchor(buf->formitem, l->linenumber, pos);
		if (a == NULL)
		    break;
		spos = a->start.pos;
		epos = a->end.pos;
	    }
	    if (a->start.line != a->end.line || spos > epos || epos >= l->len ||
		spos < 0 || epos < 0 || COLPOS(l, epos) < col)
		break;
	    pos = form_update_line(l, &p, spos, epos, COLPOS(l, epos) - col,
				   rows > 1,
				   form->type == FORM_INPUT_PASSWORD);
	    if (pos != epos) {
		shiftAnchorPosition(buf->href, buf->hmarklist,
				    a->start.line, spos, pos - epos);
		shiftAnchorPosition(buf->name, buf->hmarklist,
				    a->start.line, spos, pos - epos);
		shiftAnchorPosition(buf->img, buf->hmarklist,
				    a->start.line, spos, pos - epos);
		shiftAnchorPosition(buf->formitem, buf->hmarklist,
				    a->start.line, spos, pos - epos);
	    }
	}
	break;
    }
    copyBuffer(buf, &save);
    arrangeLine(buf);
}
