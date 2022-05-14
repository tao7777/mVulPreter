void qrio_prstcfg(u8 bit, u8 mode)
{
	u32 prstcfg;
	u8 i;
	void __iomem *qrio_base = (void *)CONFIG_SYS_QRIO_BASE;

 	prstcfg = in_be32(qrio_base + PRSTCFG_OFF);
 
 	for (i = 0; i < 2; i++) {
		if (mode & (1<<i))
			set_bit(2*bit+i, &prstcfg);
 		else
			clear_bit(2*bit+i, &prstcfg);
 	}
 
 	out_be32(qrio_base + PRSTCFG_OFF, prstcfg);
}
