void set_scl(int state)
{
	qrio_set_opendrain_gpio(DEBLOCK_PORT1, DEBLOCK_SCL1, state);
}
