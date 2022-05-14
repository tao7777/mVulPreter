 static void php_snmp_object_free_storage(void *object TSRMLS_DC)
 {
        php_snmp_object *intern = (php_snmp_object *)object;
        if (!intern) {
                return;
        }

        netsnmp_session_free(&(intern->session));
 
        zend_object_std_dtor(&intern->zo TSRMLS_CC);
        efree(intern);
 }
