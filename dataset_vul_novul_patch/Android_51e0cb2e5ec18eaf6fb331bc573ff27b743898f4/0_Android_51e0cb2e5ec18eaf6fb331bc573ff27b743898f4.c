xmlParse3986Port(xmlURIPtr uri, const char **str)
{
 const char *cur = *str;
 unsigned port = 0; /* unsigned for defined overflow behavior */

 if (ISA_DIGIT(cur)) {
 while (ISA_DIGIT(cur)) {
	    port = port * 10 + (*cur - '0');


 	    cur++;
 	}
 	if (uri != NULL)
	    uri->port = port & USHRT_MAX; /* port value modulo INT_MAX+1 */
 	*str = cur;
 	return(0);
     }
 return(1);
}
