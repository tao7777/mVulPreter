static void i2c_deblock_gpio_cfg(void)
{
	/* set I2C bus 1 deblocking GPIOs input, but 0 value for open drain */
	qrio_gpio_direction_input(DEBLOCK_PORT1, DEBLOCK_SCL1);
	qrio_gpio_direction_input(DEBLOCK_PORT1, DEBLOCK_SDA1);
	qrio_set_gpio(DEBLOCK_PORT1, DEBLOCK_SCL1, 0);
	qrio_set_gpio(DEBLOCK_PORT1, DEBLOCK_SDA1, 0);
}
