 int trigger_fpga_config(void)
 {
 	int ret = 0;
 
	skip = 0;
#ifndef CONFIG_KM_FPGA_FORCE_CONFIG
 	/* if the FPGA is already configured, we do not want to
 	 * reconfigure it */
 	skip = 0;
	if (fpga_done()) {
		printf("PCIe FPGA config: skipped\n");
 		skip = 1;
 		return 0;
 	}
#endif /* CONFIG_KM_FPGA_FORCE_CONFIG */
 
 	if (check_boco2()) {
 		/* we have a BOCO2, this has to be triggered here */

		/* make sure the FPGA_can access the EEPROM */
		ret = boco_clear_bits(SPI_REG, CFG_EEPROM);
		if (ret)
			return ret;

		/* trigger the config start */
		ret = boco_clear_bits(SPI_REG, FPGA_PROG | FPGA_INIT_B);
		if (ret)
			return ret;

		/* small delay for the pulse */
		udelay(10);

		/* up signal for pulse end */
		ret = boco_set_bits(SPI_REG, FPGA_PROG);
		if (ret)
			return ret;

		/* finally, raise INIT_B to remove the config delay */
		ret = boco_set_bits(SPI_REG, FPGA_INIT_B);
		if (ret)
			return ret;

	} else {
		/* we do it the old way, with the gpio pin */
		kw_gpio_set_valid(KM_XLX_PROGRAM_B_PIN, 1);
		kw_gpio_direction_output(KM_XLX_PROGRAM_B_PIN, 0);
		/* small delay for the pulse */
		udelay(10);
		kw_gpio_direction_input(KM_XLX_PROGRAM_B_PIN);
	}

	return 0;
}
