rdpsnd_process_training(STREAM in)
{
 	uint16 tick;
 	uint16 packsize;
 	STREAM out;
 
 	in_uint16_le(in, tick);
 	in_uint16_le(in, packsize);

	logger(Sound, Debug, "rdpsnd_process_training(), tick=0x%04x", (unsigned) tick);

	out = rdpsnd_init_packet(SNDC_TRAINING, 4);
	out_uint16_le(out, tick);
	out_uint16_le(out, packsize);
	s_mark_end(out);
	rdpsnd_send(out);
}
