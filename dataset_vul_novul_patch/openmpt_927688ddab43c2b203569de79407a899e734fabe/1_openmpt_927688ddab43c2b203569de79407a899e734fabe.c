 LIBOPENMPT_MODPLUG_API unsigned int ModPlug_InstrumentName(ModPlugFile* file, unsigned int qual, char* buff)
 {
 	const char* str;
	unsigned int retval;
	size_t tmpretval;
 	if(!file) return 0;
 	str = openmpt_module_get_instrument_name(file->mod,qual-1);
	if(!str){
		if(buff){
			*buff = '\0';
		}
		return 0;
	}
	tmpretval = strlen(str);
	if(tmpretval>=INT_MAX){
		tmpretval = INT_MAX-1;
 	}
	retval = (int)tmpretval;
 	if(buff){
		memcpy(buff,str,retval+1);
		buff[retval] = '\0';
 	}
	openmpt_free_string(str);
	return retval;
 }
