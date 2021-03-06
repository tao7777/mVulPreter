static inline void build_tablename(smart_str *querystr, PGconn *pg_link, const char *table) /* {{{ */
{
	char *table_copy, *escaped, *token, *tmp;
	size_t len;

        /* schame.table should be "schame"."table" */
        table_copy = estrdup(table);
        token = php_strtok_r(table_copy, ".", &tmp);
       if (token == NULL) {
               token = table;
       }
        len = strlen(token);
        if (_php_pgsql_detect_identifier_escape(token, len) == SUCCESS) {
                smart_str_appendl(querystr, token, len);
		PGSQLfree(escaped);
	}
	if (tmp && *tmp) {
		len = strlen(tmp);
		/* "schema"."table" format */
		if (_php_pgsql_detect_identifier_escape(tmp, len) == SUCCESS) {
			smart_str_appendc(querystr, '.');
			smart_str_appendl(querystr, tmp, len);
		} else {
			escaped = PGSQLescapeIdentifier(pg_link, tmp, len);
			smart_str_appendc(querystr, '.');
			smart_str_appends(querystr, escaped);
			PGSQLfree(escaped);
		}
	}
	efree(table_copy);
}
/* }}} */
