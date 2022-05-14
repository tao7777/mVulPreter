static char *decode_text_string(const char *str, size_t str_len)
{
    int   idx, is_hex, is_utf16be, ascii_idx;
    char *ascii, hex_buf[5] = {0};

    is_hex = is_utf16be = idx = ascii_idx = 0;

     /* Regular encoding */
     if (str[0] == '(')
     {
        ascii = safe_calloc(strlen(str) + 1);
         strncpy(ascii, str, strlen(str) + 1);
         return ascii;
     }
    else if (str[0] == '<')
    {
        is_hex = 1;
        ++idx;
    }
    
    /* Text strings can be either PDFDocEncoding or UTF-16BE */
    if (is_hex && (str_len > 5) && 
        (str[idx] == 'F') && (str[idx+1] == 'E') &&
        (str[idx+2] == 'F') && (str[idx+3] == 'F'))
    {
        is_utf16be = 1;
        idx += 4;
    }
    else
       return NULL;
 
     /* Now decode as hex */
    ascii = safe_calloc(str_len);
     for ( ; idx<str_len; ++idx)
     {
         hex_buf[0] = str[idx++];
        hex_buf[1] = str[idx++];
        hex_buf[2] = str[idx++];
        hex_buf[3] = str[idx];
        ascii[ascii_idx++] = strtol(hex_buf, NULL, 16);
    }

    return ascii;
}
