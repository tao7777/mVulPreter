XdmcpGenerateKey (XdmAuthKeyPtr key)
{
 #ifndef HAVE_ARC4RANDOM_BUF
     long    lowbits, highbits;
 
     srandom ((int)getpid() ^ time((Time_t *)0));
    highbits = random ();
     highbits = random ();
     getbits (lowbits, key->data);
     getbits (highbits, key->data + 4);
#else
     arc4random_buf(key->data, 8);
#endif
 }
