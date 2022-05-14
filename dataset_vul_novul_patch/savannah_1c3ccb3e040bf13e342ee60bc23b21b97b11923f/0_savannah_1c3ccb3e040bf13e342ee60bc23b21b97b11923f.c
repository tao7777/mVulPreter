asn1_get_bit_der (const unsigned char *der, int der_len,
 		  int *ret_len, unsigned char *str, int str_size,
 		  int *bit_len)
 {
  int len_len = 0, len_byte;
 
   if (der_len <= 0)
     return ASN1_GENERIC_ERROR;
  len_byte = asn1_get_length_der (der, der_len, &len_len) - 1;
  if (len_byte < 0)
    return ASN1_DER_ERROR;
 
   *ret_len = len_byte + len_len + 1;
   *bit_len = len_byte * 8 - der[len_len];
  
  if (*bit_len <= 0)
    return ASN1_DER_ERROR;
 
   if (str_size >= len_byte)
     memcpy (str, der + len_len + 1, len_byte);
    }
