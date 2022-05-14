static struct ion_handle *ion_handle_get_by_id(struct ion_client *client,
static struct ion_handle *ion_handle_get_by_id_nolock(struct ion_client *client,
 						int id)
 {
 	struct ion_handle *handle;
 
 	handle = idr_find(&client->idr, id);
 	if (handle)
 		ion_handle_get(handle);
 
 	return handle ? handle : ERR_PTR(-EINVAL);
 }
