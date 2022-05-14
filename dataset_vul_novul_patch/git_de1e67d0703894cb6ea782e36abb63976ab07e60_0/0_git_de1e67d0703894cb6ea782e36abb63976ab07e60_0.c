static void show_object(struct object *obj,
static void show_object(struct object *obj, const char *name, void *data)
 {
 	add_preferred_base_object(name);
 	add_object_entry(obj->oid.hash, obj->type, name, 0);
 	obj->flags |= OBJECT_ADDED;
 }
