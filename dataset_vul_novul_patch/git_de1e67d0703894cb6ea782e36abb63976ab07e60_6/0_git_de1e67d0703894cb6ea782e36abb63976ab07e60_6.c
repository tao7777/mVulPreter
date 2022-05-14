static void show_object(struct object *object, struct strbuf *path,
static void show_object(struct object *object, const char *name, void *data)
 {
 	struct bitmap *base = data;
 	bitmap_set(base, find_object_pos(object->oid.hash));
	mark_as_seen(object);
}
