 static void mark_commit(struct commit *c, void *data)
 {
	mark_object(&c->object, NULL, NULL, data);
 }
