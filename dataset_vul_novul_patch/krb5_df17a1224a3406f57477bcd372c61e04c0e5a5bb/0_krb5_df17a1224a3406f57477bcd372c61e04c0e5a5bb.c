bool_t xdr_nullstring(XDR *xdrs, char **objp)
{
     u_int size;

     if (xdrs->x_op == XDR_ENCODE) {
	  if (*objp == NULL)
	       size = 0;
	  else
	       size = strlen(*objp) + 1;
     }
     if (! xdr_u_int(xdrs, &size)) {
	  return FALSE;
	}
     switch (xdrs->x_op) {
     case XDR_DECODE:
	  if (size == 0) {
	       *objp = NULL;
	       return TRUE;
	  } else if (*objp == NULL) {
	       *objp = (char *) mem_alloc(size);
	       if (*objp == NULL) {
		    errno = ENOMEM;
 		    return FALSE;
 	       }
 	  }
	  if (!xdr_opaque(xdrs, *objp, size))
		  return FALSE;
	  /* Check that the unmarshalled bytes are a C string. */
	  if ((*objp)[size - 1] != '\0')
		  return FALSE;
	  if (memchr(*objp, '\0', size - 1) != NULL)
		  return FALSE;
	  return TRUE;
 
      case XDR_ENCODE:
 	  if (size != 0)
	       return (xdr_opaque(xdrs, *objp, size));
	  return TRUE;

     case XDR_FREE:
	  if (*objp != NULL)
	       mem_free(*objp, size);
	  *objp = NULL;
	  return TRUE;
     }

     return FALSE;
}
