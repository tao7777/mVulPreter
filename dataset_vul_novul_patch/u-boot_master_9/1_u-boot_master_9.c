 static void toggle_fpga_eeprom_bus(bool cpu_own)
 {
	qrio_gpio_direction_output(GPIO_A, PROM_SEL_L, !cpu_own);
 }
