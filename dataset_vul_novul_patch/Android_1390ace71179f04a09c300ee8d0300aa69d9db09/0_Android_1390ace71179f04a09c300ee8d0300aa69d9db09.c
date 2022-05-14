valid_length(uint8_t option, int dl, int *type)
{
 const struct dhcp_opt *opt;
 ssize_t sz;

 if (dl == 0)
 return -1;

 for (opt = dhcp_opts; opt->option; opt++) {
 if (opt->option != option)
 continue;

 
 		if (type)
 			*type = opt->type;
		/* The size of RFC3442 and RFC5969 options is checked at a later
		 * stage in the code */
 		if (opt->type == 0 ||
 		    opt->type & (STRING | RFC3442 | RFC5969))
 			return 0;
		/* The code does not use SINT16 / SINT32 together with ARRAY.
		 * It is however far easier to reason about the code if all
		 * possible array elements are included, and also does not code
		 * any additional CPU resources. sizeof(uintXX_t) ==
		 * sizeof(intXX_t) can be assumed. */
 		sz = 0;
		if (opt->type & (UINT32 | SINT32 | IPV4))
 			sz = sizeof(uint32_t);
		else if (opt->type & (UINT16 | SINT16))
 			sz = sizeof(uint16_t);
		else if (opt->type & UINT8)
 			sz = sizeof(uint8_t);
		if (opt->type & ARRAY) {
			/* The result of modulo zero is undefined. There are no
			 * options defined in this file that do not match one of
			 * the if-clauses above, so the following is not really
			 * necessary. However, to avoid confusion and unexpected
			 * behavior if the defined options are ever extended,
			 * returning false here seems sensible. */
			if (!sz) return -1;
			return (dl % sz == 0) ? 0 : -1;
		}
		return (sz == dl) ? 0 : -1;
 	}
 
 	/* unknown option, so let it pass */
 return 0;
}
