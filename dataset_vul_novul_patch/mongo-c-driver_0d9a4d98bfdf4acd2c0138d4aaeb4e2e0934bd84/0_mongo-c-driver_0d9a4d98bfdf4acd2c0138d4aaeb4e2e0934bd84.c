_bson_iter_next_internal (bson_iter_t *iter,    /* INOUT */
                          uint32_t next_keylen, /* IN */
                          const char **key,     /* OUT */
                          uint32_t *bson_type,  /* OUT */
                          bool *unsupported)    /* OUT */
{
   const uint8_t *data;
   uint32_t o;
   unsigned int len;

   BSON_ASSERT (iter);

   *unsupported = false;

   if (!iter->raw) {
      *key = NULL;
      *bson_type = BSON_TYPE_EOD;
      return false;
   }

   data = iter->raw;
   len = iter->len;

   iter->off = iter->next_off;
   iter->type = iter->off;
   iter->key = iter->off + 1;
   iter->d1 = 0;
   iter->d2 = 0;
   iter->d3 = 0;
   iter->d4 = 0;

   if (next_keylen == 0) {
      /* iterate from start to end of NULL-terminated key string */
      for (o = iter->key; o < len; o++) {
         if (!data[o]) {
            iter->d1 = ++o;
            goto fill_data_fields;
         }
      }
   } else {
      o = iter->key + next_keylen + 1;
      iter->d1 = o;
      goto fill_data_fields;
   }

   goto mark_invalid;

fill_data_fields:

   *key = bson_iter_key_unsafe (iter);
   *bson_type = ITER_TYPE (iter);

   switch (*bson_type) {
   case BSON_TYPE_DATE_TIME:
   case BSON_TYPE_DOUBLE:
   case BSON_TYPE_INT64:
   case BSON_TYPE_TIMESTAMP:
      iter->next_off = o + 8;
      break;
   case BSON_TYPE_CODE:
   case BSON_TYPE_SYMBOL:
   case BSON_TYPE_UTF8: {
      uint32_t l;

      if ((o + 4) >= len) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->d2 = o + 4;
      memcpy (&l, iter->raw + iter->d1, sizeof (l));
      l = BSON_UINT32_FROM_LE (l);

      if (l > (len - (o + 4))) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->next_off = o + 4 + l;

      /*
       * Make sure the string length includes the NUL byte.
       */
      if (BSON_UNLIKELY ((l == 0) || (iter->next_off >= len))) {
         iter->err_off = o;
         goto mark_invalid;
      }

      /*
       * Make sure the last byte is a NUL byte.
       */
      if (BSON_UNLIKELY ((iter->raw + iter->d2)[l - 1] != '\0')) {
         iter->err_off = o + 4 + l - 1;
         goto mark_invalid;
      }
   } break;
   case BSON_TYPE_BINARY: {
      bson_subtype_t subtype;
      uint32_t l;

      if (o >= (len - 4)) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->d2 = o + 4;
      iter->d3 = o + 5;

       memcpy (&l, iter->raw + iter->d1, sizeof (l));
       l = BSON_UINT32_FROM_LE (l);
 
      if (l >= (len - o - 4)) {
          iter->err_off = o;
          goto mark_invalid;
       }

      subtype = *(iter->raw + iter->d2);

      if (subtype == BSON_SUBTYPE_BINARY_DEPRECATED) {
         int32_t binary_len;

         if (l < 4) {
            iter->err_off = o;
            goto mark_invalid;
         }

         /* subtype 2 has a redundant length header in the data */
         memcpy (&binary_len, (iter->raw + iter->d3), sizeof (binary_len));
         binary_len = BSON_UINT32_FROM_LE (binary_len);
         if (binary_len + 4 != l) {
            iter->err_off = iter->d3;
            goto mark_invalid;
         }
      }

      iter->next_off = o + 5 + l;
   } break;
   case BSON_TYPE_ARRAY:
   case BSON_TYPE_DOCUMENT: {
      uint32_t l;

      if (o >= (len - 4)) {
         iter->err_off = o;
         goto mark_invalid;
      }

      memcpy (&l, iter->raw + iter->d1, sizeof (l));
      l = BSON_UINT32_FROM_LE (l);

      if ((l > len) || (l > (len - o))) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->next_off = o + l;
   } break;
   case BSON_TYPE_OID:
      iter->next_off = o + 12;
      break;
   case BSON_TYPE_BOOL: {
      char val;

      if (iter->d1 >= len) {
         iter->err_off = o;
         goto mark_invalid;
      }

      memcpy (&val, iter->raw + iter->d1, 1);
      if (val != 0x00 && val != 0x01) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->next_off = o + 1;
   } break;
   case BSON_TYPE_REGEX: {
      bool eor = false;
      bool eoo = false;

      for (; o < len; o++) {
         if (!data[o]) {
            iter->d2 = ++o;
            eor = true;
            break;
         }
      }

      if (!eor) {
         iter->err_off = iter->next_off;
         goto mark_invalid;
      }

      for (; o < len; o++) {
         if (!data[o]) {
            eoo = true;
            break;
         }
      }

      if (!eoo) {
         iter->err_off = iter->next_off;
         goto mark_invalid;
      }

      iter->next_off = o + 1;
   } break;
   case BSON_TYPE_DBPOINTER: {
      uint32_t l;

      if (o >= (len - 4)) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->d2 = o + 4;
      memcpy (&l, iter->raw + iter->d1, sizeof (l));
      l = BSON_UINT32_FROM_LE (l);

      /* Check valid string length. l counts '\0' but not 4 bytes for itself. */
      if (l == 0 || l > (len - o - 4)) {
         iter->err_off = o;
         goto mark_invalid;
      }

      if (*(iter->raw + o + l + 3)) {
         /* not null terminated */
         iter->err_off = o + l + 3;
         goto mark_invalid;
      }

      iter->d3 = o + 4 + l;
      iter->next_off = o + 4 + l + 12;
   } break;
   case BSON_TYPE_CODEWSCOPE: {
      uint32_t l;
      uint32_t doclen;

      if ((len < 19) || (o >= (len - 14))) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->d2 = o + 4;
      iter->d3 = o + 8;

      memcpy (&l, iter->raw + iter->d1, sizeof (l));
      l = BSON_UINT32_FROM_LE (l);

      if ((l < 14) || (l >= (len - o))) {
         iter->err_off = o;
         goto mark_invalid;
      }

      iter->next_off = o + l;

      if (iter->next_off >= len) {
         iter->err_off = o;
         goto mark_invalid;
      }

      memcpy (&l, iter->raw + iter->d2, sizeof (l));
      l = BSON_UINT32_FROM_LE (l);

      if (l == 0 || l >= (len - o - 4 - 4)) {
         iter->err_off = o;
         goto mark_invalid;
      }

      if ((o + 4 + 4 + l + 4) >= iter->next_off) {
         iter->err_off = o + 4;
         goto mark_invalid;
      }

      iter->d4 = o + 4 + 4 + l;
      memcpy (&doclen, iter->raw + iter->d4, sizeof (doclen));
      doclen = BSON_UINT32_FROM_LE (doclen);

      if ((o + 4 + 4 + l + doclen) != iter->next_off) {
         iter->err_off = o + 4 + 4 + l;
         goto mark_invalid;
      }
   } break;
   case BSON_TYPE_INT32:
      iter->next_off = o + 4;
      break;
   case BSON_TYPE_DECIMAL128:
      iter->next_off = o + 16;
      break;
   case BSON_TYPE_MAXKEY:
   case BSON_TYPE_MINKEY:
   case BSON_TYPE_NULL:
   case BSON_TYPE_UNDEFINED:
      iter->next_off = o;
      break;
   default:
      *unsupported = true;
   /* FALL THROUGH */
   case BSON_TYPE_EOD:
      iter->err_off = o;
      goto mark_invalid;
   }

   /*
    * Check to see if any of the field locations would overflow the
    * current BSON buffer. If so, set the error location to the offset
    * of where the field starts.
    */
   if (iter->next_off >= len) {
      iter->err_off = o;
      goto mark_invalid;
   }

   iter->err_off = 0;

   return true;

mark_invalid:
   iter->raw = NULL;
   iter->len = 0;
   iter->next_off = 0;

   return false;
}
