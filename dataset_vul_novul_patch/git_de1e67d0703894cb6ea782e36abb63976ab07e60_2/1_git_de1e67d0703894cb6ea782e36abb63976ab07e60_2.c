static void show_object(struct object *obj,
			struct strbuf *path, const char *component,
			void *cb_data)
 {
 	struct rev_list_info *info = cb_data;
	finish_object(obj, path, component, cb_data);
 	if (info->flags & REV_LIST_QUIET)
 		return;
	show_object_with_name(stdout, obj, path, component);
 }
