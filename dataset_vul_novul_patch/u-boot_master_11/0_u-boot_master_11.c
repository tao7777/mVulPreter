static int wait_for_fpga_config(void)
{
	int ret = 0, done;
	/* approx 5 s */
	u32 timeout = 500000;
 
 	printf("PCIe FPGA config:");
 	do {
		done = qrio_get_gpio(QRIO_GPIO_A, FPGA_DONE);
 		if (timeout-- == 0) {
 			printf(" FPGA_DONE timeout\n");
 			ret = -EFAULT;
			goto err_out;
		}
		udelay(10);
	} while (!done);

	printf(" done\n");
 
 err_out:
 	/* deactive CONF_SEL and give the CPU conf EEPROM access */
	qrio_set_gpio(QRIO_GPIO_A, CONF_SEL_L, 1);
 	toggle_fpga_eeprom_bus(true);
 
 	return ret;
}
