flatpak_proxy_client_finalize (GObject *object)
{
  FlatpakProxyClient *client = FLATPAK_PROXY_CLIENT (object);

   client->proxy->clients = g_list_remove (client->proxy->clients, client);
   g_clear_object (&client->proxy);
 
  g_byte_array_free (client->auth_buffer, TRUE);
   g_hash_table_destroy (client->rewrite_reply);
   g_hash_table_destroy (client->get_owner_reply);
   g_hash_table_destroy (client->unique_id_policy);

  free_side (&client->client_side);
  free_side (&client->bus_side);

  G_OBJECT_CLASS (flatpak_proxy_client_parent_class)->finalize (object);
}
