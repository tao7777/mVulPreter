 static int remove_bond(const bt_bdaddr_t *bd_addr)
 {
    if (is_restricted_mode() && !btif_storage_is_restricted_device(bd_addr))
        return BT_STATUS_SUCCESS;

     /* sanity check */
     if (interface_ready() == FALSE)
         return BT_STATUS_NOT_READY;

 return btif_dm_remove_bond(bd_addr);
}
