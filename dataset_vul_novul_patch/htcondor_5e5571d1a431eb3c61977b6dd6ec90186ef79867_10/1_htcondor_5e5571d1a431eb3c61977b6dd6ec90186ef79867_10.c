validate_entries( bool ignore_invalid_entry ) {
	HASHITER it = hash_iter_begin( ConfigTab, TABLESIZE );
	unsigned int invalid_entries = 0;
	MyString tmp;
	MyString output = "The following configuration macros appear to contain default values that must be changed before Condor will run.  These macros are:\n";
	while( ! hash_iter_done(it) ) {
		char * val = hash_iter_value(it);
		if( strstr(val, FORBIDDEN_CONFIG_VAL) ) {
			char * name = hash_iter_key(it);
			MyString filename;
			int line_number;
			param_get_location(name, filename, line_number);
			tmp.sprintf("   %s (found on line %d of %s)\n", name, line_number, filename.Value());
			output += tmp;
			invalid_entries++;
		}
		hash_iter_next(it);
	}
	hash_iter_delete(&it);
	if(invalid_entries > 0) {
                if(ignore_invalid_entry) {
                        dprintf(D_ALWAYS, "%s", output.Value());
                } else {
                       EXCEPT(output.Value());
                }
        }
 }
