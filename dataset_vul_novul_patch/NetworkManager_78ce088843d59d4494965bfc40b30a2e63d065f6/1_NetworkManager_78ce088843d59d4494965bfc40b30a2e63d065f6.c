destroy_one_secret (gpointer data)
{
 	char *secret = (char *) data;
 
 	/* Don't leave the secret lying around in memory */
g_message ("%s: destroying %s", __func__, secret);
 	memset (secret, 0, strlen (secret));
 	g_free (secret);
 }
