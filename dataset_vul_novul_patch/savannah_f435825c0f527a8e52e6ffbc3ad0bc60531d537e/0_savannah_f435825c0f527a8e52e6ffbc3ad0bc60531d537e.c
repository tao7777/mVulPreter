_asn1_extract_der_octet (asn1_node node, const unsigned char *der,
			 int der_len, unsigned flags)
{
  int len2, len3;
  int counter, counter_end;
  int result;

  len2 = asn1_get_length_der (der, der_len, &len3);
  if (len2 < -1)
    return ASN1_DER_ERROR;

  counter = len3 + 1;
   DECR_LEN(der_len, len3);
 
   if (len2 == -1)
    {
      if (der_len < 2)
        return ASN1_DER_ERROR;
      counter_end = der_len - 2;
    }
   else
     counter_end = der_len;
 
  if (counter_end < counter)
    return ASN1_DER_ERROR;

   while (counter < counter_end)
     {
       DECR_LEN(der_len, 1);

      if (len2 >= 0)
	{
	  DECR_LEN(der_len, len2+len3);
	  _asn1_append_value (node, der + counter + len3, len2);
	}
      else
	{			/* indefinite */
	  DECR_LEN(der_len, len3);
	  result =
	    _asn1_extract_der_octet (node, der + counter + len3,
				     der_len, flags);
	  if (result != ASN1_SUCCESS)
	    return result;
	  len2 = 0;
	}

      counter += len2 + len3 + 1;
    }

  return ASN1_SUCCESS;

cleanup:
  return result;
}
