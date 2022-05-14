static void test_show_object(struct object *object,
static void test_show_object(struct object *object, const char *name,
			     void *data)
 {
 	struct bitmap_test_data *tdata = data;
 	int bitmap_pos;

	bitmap_pos = bitmap_position(object->oid.hash);
	if (bitmap_pos < 0)
		die("Object not in bitmap: %s\n", oid_to_hex(&object->oid));

	bitmap_set(tdata->base, bitmap_pos);
	display_progress(tdata->prg, ++tdata->seen);
}
