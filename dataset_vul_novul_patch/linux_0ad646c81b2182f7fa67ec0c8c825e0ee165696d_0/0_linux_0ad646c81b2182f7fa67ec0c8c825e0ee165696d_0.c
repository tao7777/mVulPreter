static int dev_get_valid_name(struct net *net,
int dev_get_valid_name(struct net *net, struct net_device *dev,
		       const char *name)
 {
 	BUG_ON(!net);
 
	if (!dev_valid_name(name))
		return -EINVAL;

	if (strchr(name, '%'))
		return dev_alloc_name_ns(net, dev, name);
	else if (__dev_get_by_name(net, name))
		return -EEXIST;
	else if (dev->name != name)
		strlcpy(dev->name, name, IFNAMSIZ);
 
 	return 0;
 }
