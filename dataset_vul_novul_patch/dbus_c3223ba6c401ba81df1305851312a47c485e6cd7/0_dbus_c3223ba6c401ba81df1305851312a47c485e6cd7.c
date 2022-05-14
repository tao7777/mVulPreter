 _dbus_header_byteswap (DBusHeader *header,
                        int         new_order)
 {
  unsigned char byte_order;

   if (header->byte_order == new_order)
     return;
 
  byte_order = _dbus_string_get_byte (&header->data, BYTE_ORDER_OFFSET);
  _dbus_assert (header->byte_order == byte_order);

   _dbus_marshal_byteswap (&_dbus_header_signature_str,
                           0, header->byte_order,
                           new_order,
                           &header->data, 0);
 
  _dbus_string_set_byte (&header->data, BYTE_ORDER_OFFSET, new_order);
   header->byte_order = new_order;
 }
