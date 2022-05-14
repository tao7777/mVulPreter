  T1_Get_Private_Dict( T1_Parser      parser,
                       PSAux_Service  psaux )
  {
    FT_Stream  stream = parser->stream;
    FT_Memory  memory = parser->root.memory;
    FT_Error   error  = FT_Err_Ok;
    FT_ULong   size;


    if ( parser->in_pfb )
    {
      /* in the case of the PFB format, the private dictionary can be  */
      /* made of several segments.  We thus first read the number of   */
      /* segments to compute the total size of the private dictionary  */
      /* then re-read them into memory.                                */
      FT_ULong   start_pos = FT_STREAM_POS();
      FT_UShort  tag;


      parser->private_len = 0;
      for (;;)
      {
        error = read_pfb_tag( stream, &tag, &size );
        if ( error )
          goto Fail;

        if ( tag != 0x8002U )
          break;

        parser->private_len += size;

        if ( FT_STREAM_SKIP( size ) )
          goto Fail;
      }

      /* Check that we have a private dictionary there */
      /* and allocate private dictionary buffer        */
      if ( parser->private_len == 0 )
      {
        FT_ERROR(( "T1_Get_Private_Dict:"
                   " invalid private dictionary section\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Fail;
      }

      if ( FT_STREAM_SEEK( start_pos )                           ||
           FT_ALLOC( parser->private_dict, parser->private_len ) )
        goto Fail;

      parser->private_len = 0;
      for (;;)
      {
        error = read_pfb_tag( stream, &tag, &size );
        if ( error || tag != 0x8002U )
        {
          error = FT_Err_Ok;
          break;
        }

        if ( FT_STREAM_READ( parser->private_dict + parser->private_len,
                             size ) )
          goto Fail;

        parser->private_len += size;
      }
    }
    else
    {
      /* We have already `loaded' the whole PFA font file into memory; */
      /* if this is a memory resource, allocate a new block to hold    */
      /* the private dict.  Otherwise, simply overwrite into the base  */
      /* dictionary block in the heap.                                 */

      /* first of all, look at the `eexec' keyword */
      FT_Byte*    cur   = parser->base_dict;
      FT_Byte*    limit = cur + parser->base_len;
      FT_Byte     c;
      FT_Pointer  pos_lf;
      FT_Bool     test_cr;


    Again:
      for (;;)
      {
        c = cur[0];
        if ( c == 'e' && cur + 9 < limit )  /* 9 = 5 letters for `eexec' + */
                                            /* whitespace + 4 chars        */
        {
          if ( cur[1] == 'e' &&
               cur[2] == 'x' &&
               cur[3] == 'e' &&
               cur[4] == 'c' )
            break;
        }
        cur++;
        if ( cur >= limit )
        {
          FT_ERROR(( "T1_Get_Private_Dict:"
                     " could not find `eexec' keyword\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Exit;
        }
      }

      /* check whether `eexec' was real -- it could be in a comment */
      /* or string (as e.g. in u003043t.gsf from ghostscript)       */

      parser->root.cursor = parser->base_dict;
      /* set limit to `eexec' + whitespace + 4 characters */
      parser->root.limit  = cur + 10;

      cur   = parser->root.cursor;
      limit = parser->root.limit;

      while ( cur < limit )
      {
        if ( *cur == 'e' && ft_strncmp( (char*)cur, "eexec", 5 ) == 0 )
          goto Found;

        T1_Skip_PS_Token( parser );
        if ( parser->root.error )
          break;
        T1_Skip_Spaces  ( parser );
        cur = parser->root.cursor;
      }

      /* we haven't found the correct `eexec'; go back and continue */
      /* searching                                                  */
 
       cur   = limit;
       limit = parser->base_dict + parser->base_len;

      if ( cur >= limit )
      {
        FT_ERROR(( "T1_Get_Private_Dict:"
                   " premature end in private dictionary\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

       goto Again;
 
       /* now determine where to write the _encrypted_ binary private  */

      /* According to the Type 1 spec, the first cipher byte must not be */
      /* an ASCII whitespace character code (blank, tab, carriage return */
      /* or line feed).  We have seen Type 1 fonts with two line feed    */
      /* characters...  So skip now all whitespace character codes.      */
      /*                                                                 */
      /* On the other hand, Adobe's Type 1 parser handles fonts just     */
      /* fine that are violating this limitation, so we add a heuristic  */
      /* test to stop at \r only if it is not used for EOL.              */

      pos_lf  = ft_memchr( cur, '\n', (size_t)( limit - cur ) );
      test_cr = FT_BOOL( !pos_lf                                       ||
                         pos_lf > ft_memchr( cur,
                                             '\r',
                                             (size_t)( limit - cur ) ) );

      while ( cur < limit                    &&
              ( *cur == ' '                ||
                *cur == '\t'               ||
                (test_cr && *cur == '\r' ) ||
                *cur == '\n'               ) )
        ++cur;
      if ( cur >= limit )
      {
        FT_ERROR(( "T1_Get_Private_Dict:"
                   " `eexec' not properly terminated\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      size = parser->base_len - (FT_ULong)( cur - parser->base_dict );

      if ( parser->in_memory )
      {
        /* note that we allocate one more byte to put a terminating `0' */
        if ( FT_ALLOC( parser->private_dict, size + 1 ) )
          goto Fail;
        parser->private_len = size;
      }
      else
      {
        parser->single_block = 1;
        parser->private_dict = parser->base_dict;
        parser->private_len  = size;
        parser->base_dict    = NULL;
        parser->base_len     = 0;
      }

      /* now determine whether the private dictionary is encoded in binary */
      /* or hexadecimal ASCII format -- decode it accordingly              */

      /* we need to access the next 4 bytes (after the final whitespace */
      /* following the `eexec' keyword); if they all are hexadecimal    */
      /* digits, then we have a case of ASCII storage                   */

      if ( cur + 3 < limit                                &&
           ft_isxdigit( cur[0] ) && ft_isxdigit( cur[1] ) &&
           ft_isxdigit( cur[2] ) && ft_isxdigit( cur[3] ) )
      {
        /* ASCII hexadecimal encoding */
        FT_ULong  len;


        parser->root.cursor = cur;
        (void)psaux->ps_parser_funcs->to_bytes( &parser->root,
                                                parser->private_dict,
                                                parser->private_len,
                                                &len,
                                                0 );
        parser->private_len = len;

        /* put a safeguard */
        parser->private_dict[len] = '\0';
      }
      else
        /* binary encoding -- copy the private dict */
        FT_MEM_MOVE( parser->private_dict, cur, size );
    }

    /* we now decrypt the encoded binary private dictionary */
    psaux->t1_decrypt( parser->private_dict, parser->private_len, 55665U );

    if ( parser->private_len < 4 )
    {
      FT_ERROR(( "T1_Get_Private_Dict:"
                 " invalid private dictionary section\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    /* replace the four random bytes at the beginning with whitespace */
    parser->private_dict[0] = ' ';
    parser->private_dict[1] = ' ';
    parser->private_dict[2] = ' ';
    parser->private_dict[3] = ' ';

    parser->root.base   = parser->private_dict;
    parser->root.cursor = parser->private_dict;
    parser->root.limit  = parser->root.cursor + parser->private_len;

  Fail:
  Exit:
    return error;
  }
