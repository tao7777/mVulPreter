bool_t xdr_krb5_tl_data(XDR *xdrs, krb5_tl_data **tl_data_head)
{
     krb5_tl_data *tl, *tl2;
     bool_t more;
     unsigned int len;

     switch (xdrs->x_op) {
     case XDR_FREE:
	  tl = tl2 = *tl_data_head;
	  while (tl) {
	       tl2 = tl->tl_data_next;
	       free(tl->tl_data_contents);
 	       free(tl);
 	       tl = tl2;
 	  }
 	  break;
 
      case XDR_ENCODE:
	  tl = *tl_data_head;
	  while (1) {
	       more = (tl != NULL);
	       if (!xdr_bool(xdrs, &more))
		    return FALSE;
	       if (tl == NULL)
		    break;
	       if (!xdr_krb5_int16(xdrs, &tl->tl_data_type))
		    return FALSE;
	       len = tl->tl_data_length;
	       if (!xdr_bytes(xdrs, (char **) &tl->tl_data_contents, &len, ~0))
		    return FALSE;
	       tl = tl->tl_data_next;
	  }
	  break;

     case XDR_DECODE:
	  tl = NULL;
	  while (1) {
	       if (!xdr_bool(xdrs, &more))
		    return FALSE;
	       if (more == FALSE)
		    break;
	       tl2 = (krb5_tl_data *) malloc(sizeof(krb5_tl_data));
	       if (tl2 == NULL)
		    return FALSE;
	       memset(tl2, 0, sizeof(krb5_tl_data));
	       if (!xdr_krb5_int16(xdrs, &tl2->tl_data_type))
		    return FALSE;
	       if (!xdr_bytes(xdrs, (char **)&tl2->tl_data_contents, &len, ~0))
		    return FALSE;
	       tl2->tl_data_length = len;

	       tl2->tl_data_next = tl;
	       tl = tl2;
	  }

	  *tl_data_head = tl;
	  break;
     }

     return TRUE;
}
