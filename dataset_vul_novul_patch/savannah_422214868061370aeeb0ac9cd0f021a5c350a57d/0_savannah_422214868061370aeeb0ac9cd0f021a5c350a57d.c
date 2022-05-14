_gnutls_ciphertext2compressed (gnutls_session_t session,
                               opaque * compress_data,
                               int compress_size,
                               gnutls_datum_t ciphertext, uint8_t type,
                               record_parameters_st * params)
{
  uint8_t MAC[MAX_HASH_SIZE];
  uint16_t c_length;
  uint8_t pad;
  int length;
  uint16_t blocksize;
  int ret, i, pad_failed = 0;
  opaque preamble[PREAMBLE_SIZE];
  int preamble_size;
  int ver = gnutls_protocol_get_version (session);
  int hash_size = _gnutls_hash_get_algo_len (params->mac_algorithm);

  blocksize = gnutls_cipher_get_block_size (params->cipher_algorithm);


  /* actual decryption (inplace)
   */
  switch (_gnutls_cipher_is_block (params->cipher_algorithm))
    {
    case CIPHER_STREAM:
      if ((ret =
           _gnutls_cipher_decrypt (&params->read.cipher_state,
                                   ciphertext.data, ciphertext.size)) < 0)
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

      if ((ret =
           _gnutls_cipher_decrypt (&params->read.cipher_state,
                                   ciphertext.data, ciphertext.size)) < 0)
        {
          gnutls_assert ();
          return ret;
        }

      /* ignore the IV in TLS 1.1.
       */
      if (_gnutls_version_has_explicit_iv
          (session->security_parameters.version))
         {
           ciphertext.size -= blocksize;
           ciphertext.data += blocksize;
         }
 
      if (ciphertext.size < hash_size)
        {
          gnutls_assert ();
          return GNUTLS_E_DECRYPTION_FAILED;
        }
       pad = ciphertext.data[ciphertext.size - 1] + 1;   /* pad */
 
       if ((int) pad > (int) ciphertext.size - hash_size)
      if ((int) pad > (int) ciphertext.size - hash_size)
        {
          gnutls_assert ();
          _gnutls_record_log
            ("REC[%p]: Short record length %d > %d - %d (under attack?)\n",
             session, pad, ciphertext.size, hash_size);
          /* We do not fail here. We check below for the
           * the pad_failed. If zero means success.
           */
          pad_failed = GNUTLS_E_DECRYPTION_FAILED;
        }

      length = ciphertext.size - hash_size - pad;

      /* Check the pading bytes (TLS 1.x)
       */
      if (_gnutls_version_has_variable_padding (ver) && pad_failed == 0)
        for (i = 2; i < pad; i++)
          {
            if (ciphertext.data[ciphertext.size - i] !=
                ciphertext.data[ciphertext.size - 1])
              pad_failed = GNUTLS_E_DECRYPTION_FAILED;
          }
      break;
    default:
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }

  if (length < 0)
    length = 0;
  c_length = _gnutls_conv_uint16 ((uint16_t) length);

  /* Pass the type, version, length and compressed through
   * MAC.
   */
  if (params->mac_algorithm != GNUTLS_MAC_NULL)
    {
      digest_hd_st td;

      ret = mac_init (&td, params->mac_algorithm,
                      params->read.mac_secret.data,
                      params->read.mac_secret.size, ver);

      if (ret < 0)
        {
          gnutls_assert ();
          return GNUTLS_E_INTERNAL_ERROR;
        }

      preamble_size =
        make_preamble (UINT64DATA
                       (params->read.sequence_number), type,
                       c_length, ver, preamble);
      mac_hash (&td, preamble, preamble_size, ver);
      if (length > 0)
        mac_hash (&td, ciphertext.data, length, ver);

      mac_deinit (&td, MAC, ver);
    }

  /* This one was introduced to avoid a timing attack against the TLS
   * 1.0 protocol.
   */
  if (pad_failed != 0)
    {
      gnutls_assert ();
      return pad_failed;
    }

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
