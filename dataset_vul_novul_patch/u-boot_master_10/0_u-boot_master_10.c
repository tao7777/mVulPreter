int trigger_fpga_config(void)
{
	int ret = 0, init_l;
	/* approx 10ms */
	u32 timeout = 10000;

	/* make sure the FPGA_can access the EEPROM */
 	toggle_fpga_eeprom_bus(false);
 
 	/* assert CONF_SEL_L to be able to drive FPGA_PROG_L */
	qrio_gpio_direction_output(QRIO_GPIO_A, CONF_SEL_L, 0);
 
 	/* trigger the config start */
	qrio_gpio_direction_output(QRIO_GPIO_A, FPGA_PROG_L, 0);
 
 	/* small delay for INIT_L line */
 	udelay(10);
 
 	/* wait for FPGA_INIT to be asserted */
 	do {
		init_l = qrio_get_gpio(QRIO_GPIO_A, FPGA_INIT_L);
 		if (timeout-- == 0) {
 			printf("FPGA_INIT timeout\n");
 			ret = -EFAULT;
			break;
		}
		udelay(10);
 	} while (init_l);
 
 	/* deassert FPGA_PROG, config should start */
	qrio_set_gpio(QRIO_GPIO_A, FPGA_PROG_L, 1);
 
 	return ret;
 }
