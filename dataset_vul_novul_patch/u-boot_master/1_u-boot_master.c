void qrio_cpuwd_flag(bool flag)
 {
 	u8 reason1;
 	void __iomem *qrio_base = (void *)CONFIG_SYS_QRIO_BASE;
 	reason1 = in_8(qrio_base + REASON1_OFF);
 	if (flag)
 		reason1 |= REASON1_CPUWD;
	else
		reason1 &= ~REASON1_CPUWD;
 	out_8(qrio_base + REASON1_OFF, reason1);
 }
