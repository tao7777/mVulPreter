 LIBOPENMPT_MODPLUG_API unsigned int ModPlug_InstrumentName(ModPlugFile* file, unsigned int qual, char* buff)
 {
 	const char* str;
	char buf[32];
 	if(!file) return 0;
 	str = openmpt_module_get_instrument_name(file->mod,qual-1);
	memset(buf,0,32);
	if(str){
		strncpy(buf,str,31);
		openmpt_free_string(str);
 	}
 	if(buff){
		strncpy(buff,buf,32);
 	}
	return (unsigned int)strlen(buf);
 }
