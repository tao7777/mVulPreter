int get_sda(void)
{
	return qrio_get_gpio(DEBLOCK_PORT1, DEBLOCK_SDA1);
}
