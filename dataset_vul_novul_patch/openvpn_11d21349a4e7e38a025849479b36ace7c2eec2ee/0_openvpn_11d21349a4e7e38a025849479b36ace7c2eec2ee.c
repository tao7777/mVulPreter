openvpn_decrypt (struct buffer *buf, struct buffer work,
		 const struct crypto_options *opt,
		 const struct frame* frame)
{
  static const char error_prefix[] = "Authenticate/Decrypt packet error";
  struct gc_arena gc;
  gc_init (&gc);

  if (buf->len > 0 && opt->key_ctx_bi)
    {
      struct key_ctx *ctx = &opt->key_ctx_bi->decrypt;
      struct packet_id_net pin;
      bool have_pin = false;

      /* Verify the HMAC */
      if (ctx->hmac)
	{
	  int hmac_len;
	  uint8_t local_hmac[MAX_HMAC_KEY_LENGTH]; /* HMAC of ciphertext computed locally */

	  hmac_ctx_reset(ctx->hmac);

	  /* Assume the length of the input HMAC */
	  hmac_len = hmac_ctx_size (ctx->hmac);

	  /* Authentication fails if insufficient data in packet for HMAC */
	  if (buf->len < hmac_len)
	    CRYPT_ERROR ("missing authentication info");

	  hmac_ctx_update (ctx->hmac, BPTR (buf) + hmac_len, BLEN (buf) - hmac_len);
 	  hmac_ctx_final (ctx->hmac, local_hmac);
 
 	  /* Compare locally computed HMAC with packet HMAC */
	  if (memcmp_constant_time (local_hmac, BPTR (buf), hmac_len))
 	    CRYPT_ERROR ("packet HMAC authentication failed");
 
 	  ASSERT (buf_advance (buf, hmac_len));
	}

      /* Decrypt packet ID + payload */

      if (ctx->cipher)
	{
	  const unsigned int mode = cipher_ctx_mode (ctx->cipher);
	  const int iv_size = cipher_ctx_iv_length (ctx->cipher);
	  uint8_t iv_buf[OPENVPN_MAX_IV_LENGTH];
	  int outlen;

	  /* initialize work buffer with FRAME_HEADROOM bytes of prepend capacity */
	  ASSERT (buf_init (&work, FRAME_HEADROOM_ADJ (frame, FRAME_HEADROOM_MARKER_DECRYPT)));

	  /* use IV if user requested it */
	  CLEAR (iv_buf);
	  if (opt->flags & CO_USE_IV)
	    {
	      if (buf->len < iv_size)
		CRYPT_ERROR ("missing IV info");
	      memcpy (iv_buf, BPTR (buf), iv_size);
	      ASSERT (buf_advance (buf, iv_size));
	    }

	  /* show the IV's initial state */
	  if (opt->flags & CO_USE_IV)
	    dmsg (D_PACKET_CONTENT, "DECRYPT IV: %s", format_hex (iv_buf, iv_size, 0, &gc));

	  if (buf->len < 1)
	    CRYPT_ERROR ("missing payload");

	  /* ctx->cipher was already initialized with key & keylen */
	  if (!cipher_ctx_reset (ctx->cipher, iv_buf))
	    CRYPT_ERROR ("cipher init failed");

	  /* Buffer overflow check (should never happen) */
	  if (!buf_safe (&work, buf->len))
	    CRYPT_ERROR ("buffer overflow");

	  /* Decrypt packet ID, payload */
	  if (!cipher_ctx_update (ctx->cipher, BPTR (&work), &outlen, BPTR (buf), BLEN (buf)))
	    CRYPT_ERROR ("cipher update failed");
	  work.len += outlen;

	  /* Flush the decryption buffer */
	  if (!cipher_ctx_final (ctx->cipher, BPTR (&work) + outlen, &outlen))
	    CRYPT_ERROR ("cipher final failed");
	  work.len += outlen;

	  dmsg (D_PACKET_CONTENT, "DECRYPT TO: %s",
	       format_hex (BPTR (&work), BLEN (&work), 80, &gc));

	  /* Get packet ID from plaintext buffer or IV, depending on cipher mode */
	  {
	    if (mode == OPENVPN_MODE_CBC)
	      {
		if (opt->packet_id)
		  {
		    if (!packet_id_read (&pin, &work, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM)))
		      CRYPT_ERROR ("error reading CBC packet-id");
		    have_pin = true;
		  }
	      }
	    else if (mode == OPENVPN_MODE_CFB || mode == OPENVPN_MODE_OFB)
	      {
		struct buffer b;

		ASSERT (opt->flags & CO_USE_IV);    /* IV and packet-ID required */
		ASSERT (opt->packet_id); /*  for this mode. */

		buf_set_read (&b, iv_buf, iv_size);
		if (!packet_id_read (&pin, &b, true))
		  CRYPT_ERROR ("error reading CFB/OFB packet-id");
		have_pin = true;
	      }
	    else /* We only support CBC, CFB, or OFB modes right now */
	      {
		ASSERT (0);
	      }
	  }
	}
      else
	{
	  work = *buf;
	  if (opt->packet_id)
	    {
	      if (!packet_id_read (&pin, &work, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM)))
		CRYPT_ERROR ("error reading packet-id");
	      have_pin = !BOOL_CAST (opt->flags & CO_IGNORE_PACKET_ID);
	    }
	}
      
      if (have_pin)
	{
	  packet_id_reap_test (&opt->packet_id->rec);
	  if (packet_id_test (&opt->packet_id->rec, &pin))
	    {
	      packet_id_add (&opt->packet_id->rec, &pin);
	      if (opt->pid_persist && (opt->flags & CO_PACKET_ID_LONG_FORM))
		packet_id_persist_save_obj (opt->pid_persist, opt->packet_id);
	    }
	  else
	    {
	      if (!(opt->flags & CO_MUTE_REPLAY_WARNINGS))
	      msg (D_REPLAY_ERRORS, "%s: bad packet ID (may be a replay): %s -- see the man page entry for --no-replay and --replay-window for more info or silence this warning with --mute-replay-warnings",
		   error_prefix, packet_id_net_print (&pin, true, &gc));
	      goto error_exit;
	    }
	}
      *buf = work;
    }

  gc_free (&gc);
  return true;

 error_exit:
  crypto_clear_error();
  buf->len = 0;
  gc_free (&gc);
  return false;
}
