void traverse_commit_list(struct rev_info *revs,
			  show_commit_fn show_commit,
			  show_object_fn show_object,
			  void *data)
{
	int i;
	struct commit *commit;
	struct strbuf base;

	strbuf_init(&base, PATH_MAX);
	while ((commit = get_revision(revs)) != NULL) {
		/*
		 * an uninteresting boundary commit may not have its tree
		 * parsed yet, but we are not going to show them anyway
		 */
		if (commit->tree)
			add_pending_tree(revs, commit->tree);
		show_commit(commit, data);
	}
	for (i = 0; i < revs->pending.nr; i++) {
		struct object_array_entry *pending = revs->pending.objects + i;
		struct object *obj = pending->item;
		const char *name = pending->name;
		const char *path = pending->path;
		if (obj->flags & (UNINTERESTING | SEEN))
 			continue;
 		if (obj->type == OBJ_TAG) {
 			obj->flags |= SEEN;
			show_object(obj, NULL, name, data);
 			continue;
 		}
 		if (!path)
			path = "";
		if (obj->type == OBJ_TREE) {
			process_tree(revs, (struct tree *)obj, show_object,
				     &base, path, data);
			continue;
 		}
 		if (obj->type == OBJ_BLOB) {
 			process_blob(revs, (struct blob *)obj, show_object,
				     NULL, path, data);
 			continue;
 		}
 		die("unknown pending object %s (%s)",
		    oid_to_hex(&obj->oid), name);
	}
	object_array_clear(&revs->pending);
	strbuf_release(&base);
}
