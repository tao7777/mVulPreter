int create_user_ns(struct cred *new)
{
	struct user_namespace *ns, *parent_ns = new->user_ns;
	kuid_t owner = new->euid;
 	kgid_t group = new->egid;
 	int ret;
 
	/*
	 * Verify that we can not violate the policy of which files
	 * may be accessed that is specified by the root directory,
	 * by verifing that the root directory is at the root of the
	 * mount namespace which allows all files to be accessed.
	 */
	if (current_chrooted())
		return -EPERM;

 	/* The creator needs a mapping in the parent user namespace
 	 * or else we won't be able to reasonably tell userspace who
 	 * created a user_namespace.
	 */
	if (!kuid_has_mapping(parent_ns, owner) ||
	    !kgid_has_mapping(parent_ns, group))
		return -EPERM;

	ns = kmem_cache_zalloc(user_ns_cachep, GFP_KERNEL);
	if (!ns)
		return -ENOMEM;

	ret = proc_alloc_inum(&ns->proc_inum);
	if (ret) {
		kmem_cache_free(user_ns_cachep, ns);
		return ret;
	}

	atomic_set(&ns->count, 1);
	/* Leave the new->user_ns reference with the new user namespace. */
	ns->parent = parent_ns;
	ns->owner = owner;
	ns->group = group;

	set_cred_user_ns(new, ns);

	return 0;
}
