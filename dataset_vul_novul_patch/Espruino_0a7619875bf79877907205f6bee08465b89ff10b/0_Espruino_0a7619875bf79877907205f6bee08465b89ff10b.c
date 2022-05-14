 void jslGetTokenString(char *str, size_t len) {
   if (lex->tk == LEX_ID) {
    espruino_snprintf(str, len, "ID:%s", jslGetTokenValueAsString());
   } else if (lex->tk == LEX_STR) {
    espruino_snprintf(str, len, "String:'%s'", jslGetTokenValueAsString());
   } else
     jslTokenAsString(lex->tk, str, len);
 }
