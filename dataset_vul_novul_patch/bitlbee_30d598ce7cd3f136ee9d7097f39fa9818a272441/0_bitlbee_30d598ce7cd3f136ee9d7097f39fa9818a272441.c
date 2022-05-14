static gboolean prplcb_xfer_new_send_cb(gpointer data, gint fd, b_input_condition cond)
{
	PurpleXfer *xfer = data;
	struct im_connection *ic = purple_ic_by_pa(xfer->account);
	struct prpl_xfer_data *px = xfer->ui_data;
	PurpleBuddy *buddy;
	const char *who;

	buddy = purple_find_buddy(xfer->account, xfer->who);
	who = buddy ? purple_buddy_get_name(buddy) : xfer->who;

 	/* TODO(wilmer): After spreading some more const goodness in BitlBee,
 	   remove the evil cast below. */
 	px->ft = imcb_file_send_start(ic, (char *) who, xfer->filename, xfer->size);

	if (!px->ft) {
		return FALSE;
	}
 	px->ft->data = px;
 
 	px->ft->accept = prpl_xfer_accept;
	px->ft->canceled = prpl_xfer_canceled;
	px->ft->free = prpl_xfer_free;
	px->ft->write_request = prpl_xfer_write_request;

	return FALSE;
}
