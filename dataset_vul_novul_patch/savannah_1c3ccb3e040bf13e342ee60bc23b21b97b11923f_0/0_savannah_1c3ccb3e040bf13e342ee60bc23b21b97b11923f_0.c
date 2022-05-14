asn1_get_octet_der (const unsigned char *der, int der_len,
 		    int *ret_len, unsigned char *str, int str_size,
 		    int *str_len)
 {
  int len_len = 0;
 
   if (der_len <= 0)
     return ASN1_GENERIC_ERROR;

  /* if(str==NULL) return ASN1_SUCCESS; */
  *str_len = asn1_get_length_der (der, der_len, &len_len);

  if (*str_len < 0)
    return ASN1_DER_ERROR;

  *ret_len = *str_len + len_len;
  if (str_size >= *str_len)
    memcpy (str, der + len_len, *str_len);
  else
    {
      return ASN1_MEM_ERROR;
    }

  return ASN1_SUCCESS;
}
