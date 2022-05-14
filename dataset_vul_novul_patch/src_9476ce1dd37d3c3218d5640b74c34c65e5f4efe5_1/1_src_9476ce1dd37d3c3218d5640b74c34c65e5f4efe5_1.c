usage(void)
 {
 	fprintf(stderr,
 	    "usage: ssh-agent [-c | -s] [-Dd] [-a bind_address] [-E fingerprint_hash]\n"
	    "                 [-t life] [command [arg ...]]\n"
 	    "       ssh-agent [-c | -s] -k\n");
 	exit(1);
 }
