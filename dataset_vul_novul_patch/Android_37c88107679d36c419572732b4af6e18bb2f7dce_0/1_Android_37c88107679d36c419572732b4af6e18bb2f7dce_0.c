 static int remove_bond(const bt_bdaddr_t *bd_addr)
 {
     /* sanity check */
     if (interface_ready() == FALSE)
         return BT_STATUS_NOT_READY;

 return btif_dm_remove_bond(bd_addr);
}
