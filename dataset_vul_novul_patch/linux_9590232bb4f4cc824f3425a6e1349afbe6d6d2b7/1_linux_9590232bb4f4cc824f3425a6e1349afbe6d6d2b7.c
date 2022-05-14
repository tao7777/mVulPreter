void ion_free(struct ion_client *client, struct ion_handle *handle)
 {
 	bool valid_handle;
 
 	BUG_ON(client != handle->client);
 
	mutex_lock(&client->lock);
 	valid_handle = ion_handle_validate(client, handle);
 
 	if (!valid_handle) {
 		WARN(1, "%s: invalid handle passed to free.\n", __func__);
		mutex_unlock(&client->lock);
 		return;
 	}
 	mutex_unlock(&client->lock);
	ion_handle_put(handle);
 }
