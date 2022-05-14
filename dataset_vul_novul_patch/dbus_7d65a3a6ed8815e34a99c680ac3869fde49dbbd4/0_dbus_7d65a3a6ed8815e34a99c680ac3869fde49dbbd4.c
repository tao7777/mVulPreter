 validate_body_helper (DBusTypeReader       *reader,
                       int                   byte_order,
                       dbus_bool_t           walk_reader_to_end,
                      int                   total_depth,
                       const unsigned char  *p,
                       const unsigned char  *end,
                       const unsigned char **new_p)
 {
   int current_type;
 
  /* The spec allows arrays and structs to each nest 32, for total
   * nesting of 2*32. We want to impose the same limit on "dynamic"
   * value nesting (not visible in the signature) which is introduced
   * by DBUS_TYPE_VARIANT.
   */
  if (total_depth > (DBUS_MAXIMUM_TYPE_RECURSION_DEPTH * 2))
    {
      return DBUS_INVALID_NESTED_TOO_DEEPLY;
    }

   while ((current_type = _dbus_type_reader_get_current_type (reader)) != DBUS_TYPE_INVALID)
     {
       const unsigned char *a;
        case DBUS_TYPE_BYTE:
          ++p;
          break;

        case DBUS_TYPE_BOOLEAN:
        case DBUS_TYPE_INT16:
        case DBUS_TYPE_UINT16:
        case DBUS_TYPE_INT32:
        case DBUS_TYPE_UINT32:
        case DBUS_TYPE_UNIX_FD:
        case DBUS_TYPE_INT64:
        case DBUS_TYPE_UINT64:
        case DBUS_TYPE_DOUBLE:
          alignment = _dbus_type_get_alignment (current_type);
          a = _DBUS_ALIGN_ADDRESS (p, alignment);
          if (a >= end)
            return DBUS_INVALID_NOT_ENOUGH_DATA;
          while (p != a)
            {
              if (*p != '\0')
                return DBUS_INVALID_ALIGNMENT_PADDING_NOT_NUL;
              ++p;
            }
          
          if (current_type == DBUS_TYPE_BOOLEAN)
            {
              dbus_uint32_t v = _dbus_unpack_uint32 (byte_order,
                                                     p);
              if (!(v == 0 || v == 1))
                return DBUS_INVALID_BOOLEAN_NOT_ZERO_OR_ONE;
            }
          
          p += alignment;
          break;

        case DBUS_TYPE_ARRAY:
        case DBUS_TYPE_STRING:
        case DBUS_TYPE_OBJECT_PATH:
          {
            dbus_uint32_t claimed_len;

            a = _DBUS_ALIGN_ADDRESS (p, 4);
            if (a + 4 > end)
              return DBUS_INVALID_NOT_ENOUGH_DATA;
            while (p != a)
              {
                if (*p != '\0')
                  return DBUS_INVALID_ALIGNMENT_PADDING_NOT_NUL;
                ++p;
              }

            claimed_len = _dbus_unpack_uint32 (byte_order, p);
            p += 4;

            /* p may now be == end */
            _dbus_assert (p <= end);

            if (current_type == DBUS_TYPE_ARRAY)
              {
                int array_elem_type = _dbus_type_reader_get_element_type (reader);

                if (!_dbus_type_is_valid (array_elem_type))
                  {
                    return DBUS_INVALID_UNKNOWN_TYPECODE;
                  }

                alignment = _dbus_type_get_alignment (array_elem_type);

                a = _DBUS_ALIGN_ADDRESS (p, alignment);

                /* a may now be == end */
                if (a > end)
                  return DBUS_INVALID_NOT_ENOUGH_DATA;

                while (p != a)
                  {
                    if (*p != '\0')
                      return DBUS_INVALID_ALIGNMENT_PADDING_NOT_NUL;
                    ++p;
                  }
              }

            if (claimed_len > (unsigned long) (end - p))
              return DBUS_INVALID_LENGTH_OUT_OF_BOUNDS;

            if (current_type == DBUS_TYPE_OBJECT_PATH)
              {
                DBusString str;
                _dbus_string_init_const_len (&str, p, claimed_len);
                if (!_dbus_validate_path (&str, 0,
                                          _dbus_string_get_length (&str)))
                  return DBUS_INVALID_BAD_PATH;

                p += claimed_len;
              }
            else if (current_type == DBUS_TYPE_STRING)
              {
                DBusString str;
                _dbus_string_init_const_len (&str, p, claimed_len);
                if (!_dbus_string_validate_utf8 (&str, 0,
                                                 _dbus_string_get_length (&str)))
                  return DBUS_INVALID_BAD_UTF8_IN_STRING;

                p += claimed_len;
              }
            else if (current_type == DBUS_TYPE_ARRAY && claimed_len > 0)
              {
                DBusTypeReader sub;
                DBusValidity validity;
                const unsigned char *array_end;
                int array_elem_type;

                if (claimed_len > DBUS_MAXIMUM_ARRAY_LENGTH)
                  return DBUS_INVALID_ARRAY_LENGTH_EXCEEDS_MAXIMUM;
                
                /* Remember that the reader is types only, so we can't
                 * use it to iterate over elements. It stays the same
                 * for all elements.
                 */
                _dbus_type_reader_recurse (reader, &sub);

                array_end = p + claimed_len;

                array_elem_type = _dbus_type_reader_get_element_type (reader);

                /* avoid recursive call to validate_body_helper if this is an array
                 * of fixed-size elements
                 */ 
                if (dbus_type_is_fixed (array_elem_type))
                  {
                    /* bools need to be handled differently, because they can
                     * have an invalid value
                     */
                    if (array_elem_type == DBUS_TYPE_BOOLEAN)
                      {
                        dbus_uint32_t v;
                        alignment = _dbus_type_get_alignment (array_elem_type);

                        while (p < array_end)
                          {
                            v = _dbus_unpack_uint32 (byte_order, p);

                            if (!(v == 0 || v == 1))
                              return DBUS_INVALID_BOOLEAN_NOT_ZERO_OR_ONE;

                            p += alignment;
                          }
                      }

                    else
                      {
                        p = array_end;
                      }
                  }

                else
                  {
                    while (p < array_end)
                      {
                        validity = validate_body_helper (&sub, byte_order, FALSE, p, end, &p);
                        if (validity != DBUS_VALID)
                          return validity;
                      }
                  }

                if (p != array_end)
                  return DBUS_INVALID_ARRAY_LENGTH_INCORRECT;
              }

            /* check nul termination */
                   {
                     while (p < array_end)
                       {
                        validity = validate_body_helper (&sub, byte_order, FALSE,
                                                         total_depth + 1,
                                                         p, end, &p);
                         if (validity != DBUS_VALID)
                           return validity;
                       }
          }
          break;

        case DBUS_TYPE_SIGNATURE:
          {
            dbus_uint32_t claimed_len;
            DBusString str;
            DBusValidity validity;

            claimed_len = *p;
            ++p;

            /* 1 is for nul termination */
            if (claimed_len + 1 > (unsigned long) (end - p))
              return DBUS_INVALID_SIGNATURE_LENGTH_OUT_OF_BOUNDS;

            _dbus_string_init_const_len (&str, p, claimed_len);
            validity =
              _dbus_validate_signature_with_reason (&str, 0,
                                                    _dbus_string_get_length (&str));

            if (validity != DBUS_VALID)
              return validity;

            p += claimed_len;

            _dbus_assert (p < end);
            if (*p != DBUS_TYPE_INVALID)
              return DBUS_INVALID_SIGNATURE_MISSING_NUL;

            ++p;

            _dbus_verbose ("p = %p end = %p claimed_len %u\n", p, end, claimed_len);
          }
          break;

        case DBUS_TYPE_VARIANT:
          {
            /* 1 byte sig len, sig typecodes, align to
             * contained-type-boundary, values.
             */

            /* In addition to normal signature validation, we need to be sure
             * the signature contains only a single (possibly container) type.
             */
            dbus_uint32_t claimed_len;
            DBusString sig;
            DBusTypeReader sub;
            DBusValidity validity;
            int contained_alignment;
            int contained_type;
            DBusValidity reason;

            claimed_len = *p;
            ++p;

            /* + 1 for nul */
            if (claimed_len + 1 > (unsigned long) (end - p))
              return DBUS_INVALID_VARIANT_SIGNATURE_LENGTH_OUT_OF_BOUNDS;

            _dbus_string_init_const_len (&sig, p, claimed_len);
            reason = _dbus_validate_signature_with_reason (&sig, 0,
                                           _dbus_string_get_length (&sig));
            if (!(reason == DBUS_VALID))
              {
                if (reason == DBUS_VALIDITY_UNKNOWN_OOM_ERROR)
                  return reason;
                else 
                  return DBUS_INVALID_VARIANT_SIGNATURE_BAD;
              }

            p += claimed_len;
            
            if (*p != DBUS_TYPE_INVALID)
              return DBUS_INVALID_VARIANT_SIGNATURE_MISSING_NUL;
            ++p;

            contained_type = _dbus_first_type_in_signature (&sig, 0);
            if (contained_type == DBUS_TYPE_INVALID)
              return DBUS_INVALID_VARIANT_SIGNATURE_EMPTY;
            
            contained_alignment = _dbus_type_get_alignment (contained_type);
            
            a = _DBUS_ALIGN_ADDRESS (p, contained_alignment);
            if (a > end)
              return DBUS_INVALID_NOT_ENOUGH_DATA;
            while (p != a)
              {
                if (*p != '\0')
                  return DBUS_INVALID_ALIGNMENT_PADDING_NOT_NUL;
                ++p;
              }

            _dbus_type_reader_init_types_only (&sub, &sig, 0);

            _dbus_assert (_dbus_type_reader_get_current_type (&sub) != DBUS_TYPE_INVALID);

            validity = validate_body_helper (&sub, byte_order, FALSE, p, end, &p);
            if (validity != DBUS_VALID)
              return validity;

            if (_dbus_type_reader_next (&sub))
              return DBUS_INVALID_VARIANT_SIGNATURE_SPECIFIES_MULTIPLE_VALUES;

            _dbus_assert (_dbus_type_reader_get_current_type (&sub) == DBUS_TYPE_INVALID);
          }
          break;

        case DBUS_TYPE_DICT_ENTRY:
        case DBUS_TYPE_STRUCT:
 
             _dbus_assert (_dbus_type_reader_get_current_type (&sub) != DBUS_TYPE_INVALID);
 
            validity = validate_body_helper (&sub, byte_order, FALSE,
                                             total_depth + 1,
                                             p, end, &p);
             if (validity != DBUS_VALID)
               return validity;
 
                if (*p != '\0')
                  return DBUS_INVALID_ALIGNMENT_PADDING_NOT_NUL;
                ++p;
              }

            _dbus_type_reader_recurse (reader, &sub);

            validity = validate_body_helper (&sub, byte_order, TRUE, p, end, &p);
            if (validity != DBUS_VALID)
              return validity;
          }
          break;

        default:
          _dbus_assert_not_reached ("invalid typecode in supposedly-validated signature");
          break;
        }
