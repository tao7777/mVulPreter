seamless_process_line(const char *line, void *data)
{
	UNUSED(data);
	char *p, *l;
	char *tok1, *tok3, *tok4, *tok5, *tok6, *tok7, *tok8;
	unsigned long id, flags;
	char *endptr;

	l = xstrdup(line);
	p = l;

	logger(Core, Debug, "seamless_process_line(), got '%s'", p);

	tok1 = seamless_get_token(&p);
	(void) seamless_get_token(&p);
	tok3 = seamless_get_token(&p);
	tok4 = seamless_get_token(&p);
	tok5 = seamless_get_token(&p);
	tok6 = seamless_get_token(&p);
	tok7 = seamless_get_token(&p);
	tok8 = seamless_get_token(&p);

	if (!strcmp("CREATE", tok1))
	{
		unsigned long group, parent;
		if (!tok6)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		group = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		parent = strtoul(tok5, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok6, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_create_window(id, group, parent, flags);
	}
	else if (!strcmp("DESTROY", tok1))
	{
		if (!tok4)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_destroy_window(id, flags);

	}
	else if (!strcmp("DESTROYGRP", tok1))
	{
		if (!tok4)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_destroy_group(id, flags);
	}
	else if (!strcmp("SETICON", tok1))
	{
		int chunk, width, height, len;
		char byte[3];

		if (!tok8)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		chunk = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		width = strtoul(tok6, &endptr, 0);
		if (*endptr)
			return False;

		height = strtoul(tok7, &endptr, 0);
		if (*endptr)
			return False;

		byte[2] = '\0';
		len = 0;
		while (*tok8 != '\0')
		{
			byte[0] = *tok8;
			tok8++;
			if (*tok8 == '\0')
				return False;
			byte[1] = *tok8;
			tok8++;
 
 			icon_buf[len] = strtol(byte, NULL, 16);
 			len++;

			if ((size_t)len >= sizeof(icon_buf))
			{
				logger(Protocol, Warning, "seamless_process_line(), icon data would overrun icon_buf");
				break;
			}
 		}
 
 		ui_seamless_seticon(id, tok5, width, height, chunk, icon_buf, len);
	}
	else if (!strcmp("DELICON", tok1))
	{
		int width, height;

		if (!tok6)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		width = strtoul(tok5, &endptr, 0);
		if (*endptr)
			return False;

		height = strtoul(tok6, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_delicon(id, tok4, width, height);
	}
	else if (!strcmp("POSITION", tok1))
	{
		int x, y, width, height;

		if (!tok8)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		x = strtol(tok4, &endptr, 0);
		if (*endptr)
			return False;
		y = strtol(tok5, &endptr, 0);
		if (*endptr)
			return False;

		width = strtol(tok6, &endptr, 0);
		if (*endptr)
			return False;
		height = strtol(tok7, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok8, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_move_window(id, x, y, width, height, flags);
	}
	else if (!strcmp("ZCHANGE", tok1))
	{
		unsigned long behind;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		behind = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok5, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_restack_window(id, behind, flags);
	}
	else if (!strcmp("TITLE", tok1))
	{
		if (!tok5)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok5, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_settitle(id, tok4, flags);
	}
	else if (!strcmp("STATE", tok1))
	{
		unsigned int state;

		if (!tok5)
			return False;

		id = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		state = strtoul(tok4, &endptr, 0);
		if (*endptr)
			return False;

		flags = strtoul(tok5, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_setstate(id, state, flags);
	}
	else if (!strcmp("DEBUG", tok1))
	{
		logger(Core, Debug, "seamless_process_line(), %s", line);
	}
	else if (!strcmp("SYNCBEGIN", tok1))
	{
		if (!tok3)
			return False;

		flags = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_syncbegin(flags);
	}
	else if (!strcmp("SYNCEND", tok1))
	{
		if (!tok3)
			return False;

		flags = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		/* do nothing, currently */
	}
	else if (!strcmp("HELLO", tok1))
	{
		if (!tok3)
			return False;

		flags = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_begin(! !(flags & SEAMLESSRDP_HELLO_HIDDEN));
	}
	else if (!strcmp("ACK", tok1))
	{
		unsigned int serial;

		serial = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_ack(serial);
	}
	else if (!strcmp("HIDE", tok1))
	{
		if (!tok3)
			return False;

		flags = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_hide_desktop();
	}
	else if (!strcmp("UNHIDE", tok1))
	{
		if (!tok3)
			return False;

		flags = strtoul(tok3, &endptr, 0);
		if (*endptr)
			return False;

		ui_seamless_unhide_desktop();
	}


	xfree(l);
	return True;
}
