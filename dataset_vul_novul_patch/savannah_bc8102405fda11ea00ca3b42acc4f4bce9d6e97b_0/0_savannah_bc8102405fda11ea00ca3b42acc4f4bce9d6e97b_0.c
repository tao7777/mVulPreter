_gnutls_ciphertext2compressed (gnutls_session_t session,
			       opaque * compress_data,
			       int compress_size,
			       gnutls_datum_t ciphertext, uint8_t type)
{
  uint8_t MAC[MAX_HASH_SIZE];
  uint16_t c_length;
  uint8_t pad;
  int length;
  digest_hd_st td;
  uint16_t blocksize;
  int ret, i, pad_failed = 0;
  uint8_t major, minor;
  gnutls_protocol_t ver;
  int hash_size =
    _gnutls_hash_get_algo_len (session->security_parameters.
			       read_mac_algorithm);

  ver = gnutls_protocol_get_version (session);
  minor = _gnutls_version_get_minor (ver);
  major = _gnutls_version_get_major (ver);

  blocksize = _gnutls_cipher_get_block_size (session->security_parameters.
					     read_bulk_cipher_algorithm);

  /* initialize MAC 
   */
  ret = mac_init (&td, session->security_parameters.read_mac_algorithm,
		 session->connection_state.read_mac_secret.data,
		 session->connection_state.read_mac_secret.size, ver);

  if (ret < 0
      && session->security_parameters.read_mac_algorithm != GNUTLS_MAC_NULL)
    {
      gnutls_assert ();
       return GNUTLS_E_INTERNAL_ERROR;
     }
 
  if (ciphertext.size < (unsigned) blocksize + hash_size)
    {
      _gnutls_record_log
       ("REC[%x]: Short record length %d < %d + %d (under attack?)\n",
        session, ciphertext.size, blocksize, hash_size);
      gnutls_assert ();
      return GNUTLS_E_DECRYPTION_FAILED;
    }
 
   /* actual decryption (inplace)
    */
	{
	  gnutls_assert ();
	  return ret;
	}

      length = ciphertext.size - hash_size;

      break;
    case CIPHER_BLOCK:
      if ((ciphertext.size < blocksize) || (ciphertext.size % blocksize != 0))
	{
	  gnutls_assert ();
	  return GNUTLS_E_DECRYPTION_FAILED;
	}

      if ((ret = _gnutls_cipher_decrypt (&session->connection_state.
					 read_cipher_state,
					 ciphertext.data,
					 ciphertext.size)) < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      /* ignore the IV in TLS 1.1.
       */
      if (session->security_parameters.version >= GNUTLS_TLS1_1)
	{
	  ciphertext.size -= blocksize;
	  ciphertext.data += blocksize;

	  if (ciphertext.size == 0)
	    {
	      gnutls_assert ();
	      return GNUTLS_E_DECRYPTION_FAILED;
	    }
	}

      pad = ciphertext.data[ciphertext.size - 1] + 1;	/* pad */

      length = ciphertext.size - hash_size - pad;

      if (pad > ciphertext.size - hash_size)
	{
	  gnutls_assert ();
 
       pad = ciphertext.data[ciphertext.size - 1] + 1;  /* pad */
 
      if ((int)pad > (int)ciphertext.size - hash_size)
        {
          gnutls_assert ();
          /* We do not fail here. We check below for the
       */
      if (ver >= GNUTLS_TLS1 && pad_failed == 0)
          pad_failed = GNUTLS_E_DECRYPTION_FAILED;
        }
 
      length = ciphertext.size - hash_size - pad;

       /* Check the pading bytes (TLS 1.x)
        */
       if (ver >= GNUTLS_TLS1 && pad_failed == 0)
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }

  if (length < 0)
    length = 0;
  c_length = _gnutls_conv_uint16 ((uint16_t) length);

  /* Pass the type, version, length and compressed through
   * MAC.
   */
  if (session->security_parameters.read_mac_algorithm != GNUTLS_MAC_NULL)
    {
      _gnutls_hmac (&td,
		    UINT64DATA (session->connection_state.
				read_sequence_number), 8);

      _gnutls_hmac (&td, &type, 1);
      if (ver >= GNUTLS_TLS1)
	{			/* TLS 1.x */
	  _gnutls_hmac (&td, &major, 1);
	  _gnutls_hmac (&td, &minor, 1);
	}
      _gnutls_hmac (&td, &c_length, 2);

      if (length > 0)
	_gnutls_hmac (&td, ciphertext.data, length);

      mac_deinit (&td, MAC, ver);
    }

  /* This one was introduced to avoid a timing attack against the TLS
   * 1.0 protocol.
   */
  if (pad_failed != 0)
    return pad_failed;

  /* HMAC was not the same. 
   */
  if (memcmp (MAC, &ciphertext.data[length], hash_size) != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_DECRYPTION_FAILED;
    }

  /* copy the decrypted stuff to compress_data.
   */
  if (compress_size < length)
    {
      gnutls_assert ();
      return GNUTLS_E_DECOMPRESSION_FAILED;
    }
  memcpy (compress_data, ciphertext.data, length);

  return length;
}
