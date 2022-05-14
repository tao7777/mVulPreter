XdmcpGenerateKey (XdmAuthKeyPtr key)
 #ifndef HAVE_ARC4RANDOM_BUF

static void
emulate_getrandom_buf (char *auth, int len)
{
     long    lowbits, highbits;
 
     srandom ((int)getpid() ^ time((Time_t *)0));
    highbits = random ();
     highbits = random ();
     getbits (lowbits, key->data);
     getbits (highbits, key->data + 4);
}

static void
arc4random_buf (void *auth, int len)
{
    int	    ret;

#if HAVE_GETENTROPY
    /* weak emulation of arc4random through the getentropy libc call */
    ret = getentropy (auth, len);
    if (ret == 0)
	return;
#endif /* HAVE_GETENTROPY */

    emulate_getrandom_buf (auth, len);
}

#endif /* !defined(HAVE_ARC4RANDOM_BUF) */

void
XdmcpGenerateKey (XdmAuthKeyPtr key)
{
     arc4random_buf(key->data, 8);
 }
