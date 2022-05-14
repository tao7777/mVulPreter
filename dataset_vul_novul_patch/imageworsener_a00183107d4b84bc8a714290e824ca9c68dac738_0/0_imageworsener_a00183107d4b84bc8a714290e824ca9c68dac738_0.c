 static int find_low_bit(unsigned int x)
 {
 	int i;
 	for(i=0;i<=31;i++) {
		if(x&(1U<<(unsigned int)i)) return i;
 	}
 	return 0;
 }
