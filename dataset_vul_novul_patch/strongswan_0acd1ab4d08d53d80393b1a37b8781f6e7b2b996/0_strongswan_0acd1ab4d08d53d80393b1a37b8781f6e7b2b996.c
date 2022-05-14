static bool on_accept(private_stroke_socket_t *this, stream_t *stream)
{
	stroke_msg_t *msg;
	uint16_t len;
	FILE *out;

	/* read length */
	if (!stream->read_all(stream, &len, sizeof(len)))
	{
		if (errno != EWOULDBLOCK)
		{
			DBG1(DBG_CFG, "reading length of stroke message failed: %s",
				 strerror(errno));
                }
                return FALSE;
        }
       if (len < offsetof(stroke_msg_t, buffer))
       {
               DBG1(DBG_CFG, "invalid stroke message length %d", len);
               return FALSE;
       }
 
        /* read message (we need an additional byte to terminate the buffer) */
        msg = malloc(len + 1);
			DBG1(DBG_CFG, "reading stroke message failed: %s", strerror(errno));
		}
