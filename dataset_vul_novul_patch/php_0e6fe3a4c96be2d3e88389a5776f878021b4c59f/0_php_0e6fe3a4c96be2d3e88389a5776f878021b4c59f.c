 ZEND_METHOD(CURLFile, __wakeup)
 {
       zval *_this = getThis();

       zend_unset_property(curl_CURLFile_class, _this, "name", sizeof("name")-1 TSRMLS_CC);
       zend_update_property_string(curl_CURLFile_class, _this, "name", sizeof("name")-1, "" TSRMLS_CC);
        zend_throw_exception(NULL, "Unserialization of CURLFile instances is not allowed", 0 TSRMLS_CC);
 }
