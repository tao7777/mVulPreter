void show_object_with_name(FILE *out, struct object *obj,
	const char *p;
 
 	fprintf(out, "%s ", oid_to_hex(&obj->oid));
 	for (p = name; *p && *p != '\n'; p++)
 		fputc(*p, out);
 	fputc('\n', out);
 }
