IceGenerateMagicCookie (
 	int len
 )
 {
    char    *auth;
#ifndef HAVE_ARC4RANDOM_BUF
     long    ldata[2];
     int	    seed;
     int	    value;
     int	    i;
#endif
 
    if ((auth = malloc (len + 1)) == NULL)
	return (NULL);
#ifdef HAVE_ARC4RANDOM_BUF
    arc4random_buf(auth, len);
#else
 #ifdef ITIMER_REAL
     {
 	struct timeval  now;
    int	    i;
 	ldata[0] = now.tv_sec;
 	ldata[1] = now.tv_usec;
     }
#else
     {
 	long    time ();
 	ldata[0] = time ((long *) 0);
 	ldata[1] = getpid ();
     }
#endif
     seed = (ldata[0]) + (ldata[1] << 16);
     srand (seed);
     for (i = 0; i < len; i++)
	ldata[1] = now.tv_usec;
 	value = rand ();
 	auth[i] = value & 0xff;
     }
