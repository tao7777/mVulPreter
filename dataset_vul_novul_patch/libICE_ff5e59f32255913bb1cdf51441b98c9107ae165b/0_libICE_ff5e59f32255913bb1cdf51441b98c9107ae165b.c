IceGenerateMagicCookie (
static void
emulate_getrandom_buf (
	char *auth,
 	int len
 )
 {
     long    ldata[2];
     int	    seed;
     int	    value;
     int	    i;
 
 #ifdef ITIMER_REAL
     {
 	struct timeval  now;
    int	    i;
 	ldata[0] = now.tv_sec;
 	ldata[1] = now.tv_usec;
     }
#else /* ITIMER_REAL */
     {
 	long    time ();
 	ldata[0] = time ((long *) 0);
 	ldata[1] = getpid ();
     }
#endif /* ITIMER_REAL */
     seed = (ldata[0]) + (ldata[1] << 16);
     srand (seed);
     for (i = 0; i < len; i++)
	ldata[1] = now.tv_usec;
 	value = rand ();
 	auth[i] = value & 0xff;
     }
