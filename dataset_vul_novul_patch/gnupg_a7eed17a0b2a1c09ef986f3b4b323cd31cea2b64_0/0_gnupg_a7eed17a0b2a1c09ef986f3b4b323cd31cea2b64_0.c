parse_asntime_into_isotime (unsigned char const **buf, size_t *len,
                            ksba_isotime_t isotime)
{
  struct tag_info ti;
  gpg_error_t err;

  err = _ksba_ber_parse_tl (buf, len, &ti);
  if (err)
    ;
  else if ( !(ti.class == CLASS_UNIVERSAL
               && (ti.tag == TYPE_UTC_TIME || ti.tag == TYPE_GENERALIZED_TIME)
               && !ti.is_constructed) )
     err = gpg_error (GPG_ERR_INV_OBJ);
  else if (ti.length > *len)
    err = gpg_error (GPG_ERR_INV_BER);
   else if (!(err = _ksba_asntime_to_iso (*buf, ti.length,
                                          ti.tag == TYPE_UTC_TIME, isotime)))
     parse_skip (buf, len, &ti);
}
