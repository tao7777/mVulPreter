static void bump_cpu_timer(struct k_itimer *timer, u64 now)
{
	int i;
	u64 delta, incr;

	if (timer->it.cpu.incr == 0)
		return;

	if (now < timer->it.cpu.expires)
		return;

	incr = timer->it.cpu.incr;
	delta = now + incr - timer->it.cpu.expires;

	/* Don't use (incr*2 < delta), incr*2 might overflow. */
	for (i = 0; incr < delta - incr; i++)
		incr = incr << 1;

	for (; i >= 0; incr >>= 1, i--) {
		if (delta < incr)
 			continue;
 
 		timer->it.cpu.expires += incr;
		timer->it_overrun += 1LL << i;
 		delta -= incr;
 	}
 }
