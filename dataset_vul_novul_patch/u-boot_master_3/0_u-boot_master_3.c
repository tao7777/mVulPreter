int board_early_init_r(void)
{
	int ret = 0;
	/* Flush d-cache and invalidate i-cache of any FLASH data */
	flush_dcache();
	invalidate_icache();

	set_liodns();
	setup_qbman_portals();

	ret = trigger_fpga_config();
	if (ret)
		printf("error triggering PCIe FPGA config\n");

	/* enable the Unit LED (red) & Boot LED (on) */
	qrio_set_leds();

 	/* enable Application Buffer */
 	qrio_enable_app_buffer();
 
	return 0;
 }
