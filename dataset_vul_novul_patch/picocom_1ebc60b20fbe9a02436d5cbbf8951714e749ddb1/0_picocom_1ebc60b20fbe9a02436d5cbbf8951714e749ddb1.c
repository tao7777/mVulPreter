do_command (unsigned char c)
{
	static int dtr_up = 0;
	int newbaud, newflow, newparity, newbits;
	const char *xfr_cmd;
	char *fname;
	int r;

	switch (c) {
	case KEY_EXIT:
		return 1;
	case KEY_QUIT:
		term_set_hupcl(tty_fd, 0);
		term_flush(tty_fd);
		term_apply(tty_fd);
		term_erase(tty_fd);
		return 1;
	case KEY_STATUS:
		show_status(dtr_up);
		break;
	case KEY_PULSE:
		fd_printf(STO, "\r\n*** pulse DTR ***\r\n");
		if ( term_pulse_dtr(tty_fd) < 0 )
			fd_printf(STO, "*** FAILED\r\n");
		break;
	case KEY_TOGGLE:
		if ( dtr_up )
			r = term_lower_dtr(tty_fd);
		else
			r = term_raise_dtr(tty_fd);
		if ( r >= 0 ) dtr_up = ! dtr_up;
		fd_printf(STO, "\r\n*** DTR: %s ***\r\n", 
				  dtr_up ? "up" : "down");
		break;
	case KEY_BAUD_UP:
	case KEY_BAUD_DN:
		if (c == KEY_BAUD_UP)
			opts.baud = baud_up(opts.baud);
		else 
			opts.baud = baud_down(opts.baud);
		term_set_baudrate(tty_fd, opts.baud);
		tty_q.len = 0; term_flush(tty_fd);
		term_apply(tty_fd);
		newbaud = term_get_baudrate(tty_fd, NULL);
		if ( opts.baud != newbaud ) {
			fd_printf(STO, "\r\n*** baud: %d (%d) ***\r\n", 
					  opts.baud, newbaud);
		} else {
			fd_printf(STO, "\r\n*** baud: %d ***\r\n", opts.baud);
		}
		set_tty_write_sz(newbaud);
		break;
	case KEY_FLOW:
		opts.flow = flow_next(opts.flow);
		term_set_flowcntrl(tty_fd, opts.flow);
		tty_q.len = 0; term_flush(tty_fd);
		term_apply(tty_fd);
		newflow = term_get_flowcntrl(tty_fd);
		if ( opts.flow != newflow ) {
			fd_printf(STO, "\r\n*** flow: %s (%s) ***\r\n", 
					  flow_str[opts.flow], flow_str[newflow]);
		} else {
			fd_printf(STO, "\r\n*** flow: %s ***\r\n", 
					  flow_str[opts.flow]);
		}
		break;
	case KEY_PARITY:
		opts.parity = parity_next(opts.parity);
		term_set_parity(tty_fd, opts.parity);
		tty_q.len = 0; term_flush(tty_fd);
		term_apply(tty_fd);
		newparity = term_get_parity(tty_fd);
		if (opts.parity != newparity ) {
			fd_printf(STO, "\r\n*** parity: %s (%s) ***\r\n",
					  parity_str[opts.parity], 
					  parity_str[newparity]);
		} else {
			fd_printf(STO, "\r\n*** parity: %s ***\r\n", 
					  parity_str[opts.parity]);
		}
		break;
	case KEY_BITS:
		opts.databits = bits_next(opts.databits);
		term_set_databits(tty_fd, opts.databits);
		tty_q.len = 0; term_flush(tty_fd);
		term_apply(tty_fd);
		newbits = term_get_databits(tty_fd);
		if (opts.databits != newbits ) {
			fd_printf(STO, "\r\n*** databits: %d (%d) ***\r\n",
					  opts.databits, newbits);
		} else {
			fd_printf(STO, "\r\n*** databits: %d ***\r\n", 
					  opts.databits);
		}
		break;
	case KEY_LECHO:
		opts.lecho = ! opts.lecho;
		fd_printf(STO, "\r\n*** local echo: %s ***\r\n", 
				  opts.lecho ? "yes" : "no");
		break;
	case KEY_SEND:
	case KEY_RECEIVE:
		xfr_cmd = (c == KEY_SEND) ? opts.send_cmd : opts.receive_cmd;
		if ( xfr_cmd[0] == '\0' ) {
			fd_printf(STO, "\r\n*** command disabled ***\r\n");
			break;
		}
		fname = read_filename();
		if (fname == NULL) {
 			fd_printf(STO, "*** cannot read filename ***\r\n");
 			break;
 		}
		run_cmd(tty_fd, xfr_cmd, fname);
 		free(fname);
 		break;
 	case KEY_BREAK:
		term_break(tty_fd);
		fd_printf(STO, "\r\n*** break sent ***\r\n");
		break;
	default:
		break;
	}

	return 0;
}
