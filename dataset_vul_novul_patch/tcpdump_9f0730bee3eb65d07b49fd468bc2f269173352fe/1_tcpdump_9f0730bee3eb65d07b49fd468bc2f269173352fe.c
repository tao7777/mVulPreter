 bittok2str_internal(register const struct tok *lp, register const char *fmt,
 	   register u_int v, const char *sep)
 {
        static char buf[256]; /* our stringbuffer */
        int buflen=0;
         register u_int rotbit; /* this is the bit we rotate through all bitpositions */
         register u_int tokval;
         const char * sepstr = "";

	while (lp != NULL && lp->s != NULL) {
            tokval=lp->v;   /* load our first value */
            rotbit=1;
            while (rotbit != 0) {
                /*
                 * lets AND the rotating bit with our token value
                 * and see if we have got a match
                  */
 		if (tokval == (v&rotbit)) {
                     /* ok we have found something */
                    buflen+=snprintf(buf+buflen, sizeof(buf)-buflen, "%s%s",
                                     sepstr, lp->s);
                     sepstr = sep;
                     break;
                 }
                rotbit=rotbit<<1; /* no match - lets shift and try again */
            }
             lp++;
 	}
 
        if (buflen == 0)
             /* bummer - lets print the "unknown" message as advised in the fmt string if we got one */
             (void)snprintf(buf, sizeof(buf), fmt == NULL ? "#%08x" : fmt, v);
         return (buf);
}
