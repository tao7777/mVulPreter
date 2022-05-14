static void windows_clear_transfer_priv(struct usbi_transfer *itransfer)
{
 	struct windows_transfer_priv *transfer_priv = (struct windows_transfer_priv*)usbi_transfer_get_os_priv(itransfer);
 
 	usbi_free_fd(&transfer_priv->pollable_fd);
 	auto_release(itransfer);
 }
