static bool write_hci_command(hci_packet_t type, const void *packet, size_t length) {
 int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 if (sock == INVALID_FD)
 goto error;

 struct sockaddr_in addr;

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(0x7F000001);
   addr.sin_port = htons(8873);
  if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
     goto error;
 
  if (send(sock, &type, 1, 0) != 1)
     goto error;
 
  if (send(sock, &length, 2, 0) != 2)
     goto error;
 
  if (send(sock, packet, length, 0) != (ssize_t)length)
     goto error;
 
   close(sock);
 return true;

error:;
  close(sock);
 return false;
}
