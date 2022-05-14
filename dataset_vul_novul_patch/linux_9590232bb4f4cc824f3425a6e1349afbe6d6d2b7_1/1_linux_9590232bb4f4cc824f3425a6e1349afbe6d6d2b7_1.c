static int ion_handle_put(struct ion_handle *handle)
 {
 	struct ion_client *client = handle->client;
 	int ret;
 
 	mutex_lock(&client->lock);
	ret = kref_put(&handle->ref, ion_handle_destroy);
 	mutex_unlock(&client->lock);
 
 	return ret;
}
