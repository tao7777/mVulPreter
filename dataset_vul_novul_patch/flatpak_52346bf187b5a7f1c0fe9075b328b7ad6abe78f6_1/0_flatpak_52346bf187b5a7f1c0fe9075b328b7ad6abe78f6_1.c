flatpak_proxy_client_init (FlatpakProxyClient *client)
{
   init_side (client, &client->client_side);
   init_side (client, &client->bus_side);
 
  client->auth_buffer = g_byte_array_new ();
   client->rewrite_reply = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
   client->get_owner_reply = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
   client->unique_id_policy = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}
