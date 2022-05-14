void show_object_with_name(FILE *out, struct object *obj,
			   struct strbuf *path, const char *component)
{
	char *name = path_name(path, component);
	char *p;
 
 	fprintf(out, "%s ", oid_to_hex(&obj->oid));
 	for (p = name; *p && *p != '\n'; p++)
 		fputc(*p, out);
 	fputc('\n', out);
	free(name);
 }
