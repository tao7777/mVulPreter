static int tap_if_down(const char *devname)
{
 struct ifreq ifr;
 int sk;

    sk = socket(AF_INET, SOCK_DGRAM, 0);
 if (sk < 0)
 return -1;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, devname, IF_NAMESIZE - 1);

 
     ifr.ifr_flags &= ~IFF_UP;
 
    ioctl(sk, SIOCSIFFLAGS, (caddr_t) &ifr);
 
     close(sk);
 
 return 0;
}
