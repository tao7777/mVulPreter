 void jslGetTokenString(char *str, size_t len) {
   if (lex->tk == LEX_ID) {
    strncpy(str, "ID:", len);
    strncat(str, jslGetTokenValueAsString(), len);
   } else if (lex->tk == LEX_STR) {
    strncpy(str, "String:'", len);
    strncat(str, jslGetTokenValueAsString(), len);
    strncat(str, "'", len);
   } else
     jslTokenAsString(lex->tk, str, len);
 }
