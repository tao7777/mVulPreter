static int ion_handle_put(struct ion_handle *handle)
static int ion_handle_put_nolock(struct ion_handle *handle)
{
	int ret;

	ret = kref_put(&handle->ref, ion_handle_destroy);

	return ret;
}

int ion_handle_put(struct ion_handle *handle)
 {
 	struct ion_client *client = handle->client;
 	int ret;
 
 	mutex_lock(&client->lock);
	ret = ion_handle_put_nolock(handle);
 	mutex_unlock(&client->lock);
 
 	return ret;
}
