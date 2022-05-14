void set_sda(int state)
{
	qrio_set_opendrain_gpio(DEBLOCK_PORT1, DEBLOCK_SDA1, state);
}
