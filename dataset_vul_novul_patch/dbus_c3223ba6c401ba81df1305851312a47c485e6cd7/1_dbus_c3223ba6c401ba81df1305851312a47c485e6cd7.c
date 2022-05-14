 _dbus_header_byteswap (DBusHeader *header,
                        int         new_order)
 {
   if (header->byte_order == new_order)
     return;
 
   _dbus_marshal_byteswap (&_dbus_header_signature_str,
                           0, header->byte_order,
                           new_order,
                           &header->data, 0);
 
   header->byte_order = new_order;
 }
