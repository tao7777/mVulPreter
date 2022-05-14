int strdup_if_nonnull(char **target, char *source)
{
	if (source) {
		*target = kstrdup(source, GFP_KERNEL);
		if (!*target)
			return -ENOMEM;
	} else
		*target = NULL;
	return 0;
}
