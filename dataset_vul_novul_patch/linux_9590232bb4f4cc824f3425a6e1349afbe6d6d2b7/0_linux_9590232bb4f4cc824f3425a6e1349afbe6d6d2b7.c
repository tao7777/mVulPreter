void ion_free(struct ion_client *client, struct ion_handle *handle)
static void ion_free_nolock(struct ion_client *client, struct ion_handle *handle)
 {
 	bool valid_handle;
 
 	BUG_ON(client != handle->client);
 
 	valid_handle = ion_handle_validate(client, handle);
 
 	if (!valid_handle) {
 		WARN(1, "%s: invalid handle passed to free.\n", __func__);
 		return;
 	}
	ion_handle_put_nolock(handle);
}

void ion_free(struct ion_client *client, struct ion_handle *handle)
{
	BUG_ON(client != handle->client);

	mutex_lock(&client->lock);
	ion_free_nolock(client, handle);
 	mutex_unlock(&client->lock);
 }
