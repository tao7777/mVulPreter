 static int ldb_dn_escape_internal(char *dst, const char *src, int len)
 {
       char c;
        char *d;
       int i;
        d = dst;
 
       for (i = 0; i < len; i++){
               c = src[i];
               switch (c) {
                case ' ':
                       if (i == 0 || i == len - 1) {
                                /* if at the beginning or end
                                 * of the string then escape */
                                *d++ = '\\';
                               *d++ = c;
                        } else {
                                /* otherwise don't escape */
                               *d++ = c;
                        }
                        break;
 
				/* if at the beginning or end
				 * of the string then escape */
				*d++ = '\\';
				*d++ = *p++;					 
			} else {
				/* otherwise don't escape */
				*d++ = *p++;
			}
			break;
                case '?':
                        /* these must be escaped using \c form */
                        *d++ = '\\';
                       *d++ = c;
                        break;
 
               case ';':
               case '\r':
               case '\n':
               case '=':
               case '\0': {
                        /* any others get \XX form */
                        unsigned char v;
                        const char *hexbytes = "0123456789ABCDEF";
                       v = (const unsigned char)c;
                        *d++ = '\\';
                        *d++ = hexbytes[v>>4];
                        *d++ = hexbytes[v&0xF];
                        break;
                }
               default:
                       *d++ = c;
                }
        }
