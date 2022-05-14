static int find_high_bit(unsigned int x)
 {
 	int i;
 	for(i=31;i>=0;i--) {
		if(x&(1<<i)) return i;
 	}
 	return 0;
 }
