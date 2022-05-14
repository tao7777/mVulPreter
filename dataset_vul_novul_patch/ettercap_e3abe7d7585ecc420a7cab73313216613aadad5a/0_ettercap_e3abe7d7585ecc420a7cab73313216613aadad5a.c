FUNC_DECODER(dissector_postgresql)
{
   DECLARE_DISP_PTR(ptr);
   struct ec_session *s = NULL;
   void *ident = NULL;
   char tmp[MAX_ASCII_ADDR_LEN];
   struct postgresql_status *conn_status;

   /* don't complain about unused var */
   (void) DECODE_DATA; 
   (void) DECODE_DATALEN;
   (void) DECODED_LEN;
   
   if (FROM_CLIENT("postgresql", PACKET)) {
      if (PACKET->DATA.len < 4)
         return NULL;

      dissect_create_ident(&ident, PACKET, DISSECT_CODE(dissector_postgresql));

      /* if the session does not exist... */
      if (session_get(&s, ident, DISSECT_IDENT_LEN) == -ENOTFOUND) {
         /* search for user and database strings, look for StartupMessage  */
         unsigned char *u = memmem(ptr, PACKET->DATA.len, "user", 4);
         unsigned char *d = memmem(ptr, PACKET->DATA.len, "database", 8);
         if (!memcmp(ptr + 4, "\x00\x03\x00\x00", 4) && u && d) {
            /* create the new session */
            dissect_create_session(&s, PACKET, DISSECT_CODE(dissector_postgresql));

            /* remember the state (used later) */
            SAFE_CALLOC(s->data, 1, sizeof(struct postgresql_status));

            conn_status = (struct postgresql_status *) s->data;
            conn_status->status = WAIT_AUTH;

            /* user is always null-terminated */
            strncpy((char*)conn_status->user, (char*)(u + 5), 65);
            conn_status->user[64] = 0;

            /* database is always null-terminated */
            strncpy((char*)conn_status->database, (char*)(d + 9), 65);
            conn_status->database[64] = 0;

            /* save the session */
            session_put(s);
         }
      } else {
         conn_status = (struct postgresql_status *) s->data;
         if (conn_status->status == WAIT_RESPONSE) {

            /* check for PasswordMessage packet */
            if (ptr[0] == 'p' && conn_status->type == MD5) {
               DEBUG_MSG("\tDissector_postgresql RESPONSE type is MD5");
               if(memcmp(ptr + 1, "\x00\x00\x00\x28", 4)) {
                  DEBUG_MSG("\tDissector_postgresql BUG, expected length is 40");
                  return NULL;
               }
               if (PACKET->DATA.len < 40) {
                  DEBUG_MSG("\tDissector_postgresql BUG, expected length is 40");
                  return NULL;
               }
               memcpy(conn_status->hash, ptr + 5 + 3, 32);
               conn_status->hash[32] = 0;
               DISSECT_MSG("%s:$postgres$%s*%s*%s:%s:%d\n", conn_status->user, conn_status->user, conn_status->salt, conn_status->hash, ip_addr_ntoa(&PACKET->L3.dst, tmp), ntohs(PACKET->L4.dst));
               dissect_wipe_session(PACKET, DISSECT_CODE(dissector_postgresql));
            }
            else if (ptr[0] == 'p' && conn_status->type == CT) {
                int length;
                DEBUG_MSG("\tDissector_postgresql RESPONSE type is clear-text!");
                GET_ULONG_BE(length, ptr, 1);
               length -= 4;
               if (length < 0 || length > 65 || PACKET->DATA.len < length+5) {
                   dissect_wipe_session(PACKET, DISSECT_CODE(dissector_postgresql));
                   return NULL;
               }
               snprintf((char*)conn_status->password, length+1, "%s", (char*)(ptr + 5));
                DISSECT_MSG("PostgreSQL credentials:%s-%d:%s:%s\n", ip_addr_ntoa(&PACKET->L3.dst, tmp), ntohs(PACKET->L4.dst), conn_status->user, conn_status->password);
                dissect_wipe_session(PACKET, DISSECT_CODE(dissector_postgresql));
             }
         }
      }
   } else { /* Packets coming from the server */
      if (PACKET->DATA.len < 9)
         return NULL;
      dissect_create_ident(&ident, PACKET, DISSECT_CODE(dissector_postgresql));

      if (session_get(&s, ident, DISSECT_IDENT_LEN) == ESUCCESS) {
         conn_status = (struct postgresql_status *) s->data;
         if (conn_status->status == WAIT_AUTH &&
               ptr[0] == 'R' && !memcmp(ptr + 1, "\x00\x00\x00\x0c", 4)  &&
               !memcmp(ptr + 5, "\x00\x00\x00\x05", 4)) {

            conn_status->status = WAIT_RESPONSE;

            conn_status->type = MD5;
            DEBUG_MSG("\tDissector_postgresql AUTH type is MD5");
            hex_encode(ptr + 9, 4, conn_status->salt); /* save salt */
         }
         else if (conn_status->status == WAIT_AUTH &&
               ptr[0] == 'R' && !memcmp(ptr + 1, "\x00\x00\x00\x08", 4)  &&
               !memcmp(ptr + 5, "\x00\x00\x00\x03", 4)) {
            conn_status->status = WAIT_RESPONSE;
            conn_status->type = CT;
            DEBUG_MSG("\tDissector_postgresql AUTH type is clear-text!");
         }
      }
   }

   SAFE_FREE(ident);
   return NULL;
}
