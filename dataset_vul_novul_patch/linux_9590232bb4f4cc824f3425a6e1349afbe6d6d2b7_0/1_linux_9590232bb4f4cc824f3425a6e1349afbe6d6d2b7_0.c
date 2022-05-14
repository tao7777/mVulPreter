static struct ion_handle *ion_handle_get_by_id(struct ion_client *client,
 						int id)
 {
 	struct ion_handle *handle;
 
	mutex_lock(&client->lock);
 	handle = idr_find(&client->idr, id);
 	if (handle)
 		ion_handle_get(handle);
	mutex_unlock(&client->lock);
 
 	return handle ? handle : ERR_PTR(-EINVAL);
 }
