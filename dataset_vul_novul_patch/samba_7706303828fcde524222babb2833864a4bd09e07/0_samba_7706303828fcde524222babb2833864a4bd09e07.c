int parse_arguments(int *argc_p, const char ***argv_p)
{
	static poptContext pc;
	char *ref = lp_refuse_options(module_id);
        const char *arg, **argv = *argv_p;
        int argc = *argc_p;
        int opt;
       int orig_protect_args = protect_args;
 
        if (ref && *ref)
                set_refuse_options(ref);
		set_refuse_options("log-file*");
#ifdef ICONV_OPTION
		if (!*lp_charset(module_id))
			set_refuse_options("iconv");
#endif
	}
