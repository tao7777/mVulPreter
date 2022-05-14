  sfnt_init_face( FT_Stream      stream,
                  TT_Face        face,
                  FT_Int         face_instance_index,
                  FT_Int         num_params,
                  FT_Parameter*  params )
  {
    FT_Error      error;
    FT_Memory     memory = face->root.memory;
    FT_Library    library = face->root.driver->root.library;
    SFNT_Service  sfnt;
    FT_Int        face_index;


    /* for now, parameters are unused */
    FT_UNUSED( num_params );
    FT_UNUSED( params );


    sfnt = (SFNT_Service)face->sfnt;
    if ( !sfnt )
    {
      sfnt = (SFNT_Service)FT_Get_Module_Interface( library, "sfnt" );
      if ( !sfnt )
      {
        FT_ERROR(( "sfnt_init_face: cannot access `sfnt' module\n" ));
        return FT_THROW( Missing_Module );
      }

      face->sfnt       = sfnt;
      face->goto_table = sfnt->goto_table;
    }

    FT_FACE_FIND_GLOBAL_SERVICE( face, face->psnames, POSTSCRIPT_CMAPS );

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    if ( !face->mm )
    {
      /* we want the MM interface from the `truetype' module only */
      FT_Module  tt_module = FT_Get_Module( library, "truetype" );


      face->mm = ft_module_get_service( tt_module,
                                        FT_SERVICE_ID_MULTI_MASTERS,
                                        0 );
    }

    if ( !face->var )
    {
      /* we want the metrics variations interface */
      /* from the `truetype' module only          */
      FT_Module  tt_module = FT_Get_Module( library, "truetype" );


      face->var = ft_module_get_service( tt_module,
                                         FT_SERVICE_ID_METRICS_VARIATIONS,
                                         0 );
    }
#endif

    FT_TRACE2(( "SFNT driver\n" ));

    error = sfnt_open_font( stream, face );
    if ( error )
      return error;

    /* Stream may have changed in sfnt_open_font. */
    stream = face->root.stream;

    FT_TRACE2(( "sfnt_init_face: %08p, %d\n", face, face_instance_index ));

    face_index = FT_ABS( face_instance_index ) & 0xFFFF;

    /* value -(N+1) requests information on index N */
    if ( face_instance_index < 0 )
      face_index--;

    if ( face_index >= face->ttc_header.count )
    {
      if ( face_instance_index >= 0 )
        return FT_THROW( Invalid_Argument );
      else
        face_index = 0;
    }

    if ( FT_STREAM_SEEK( face->ttc_header.offsets[face_index] ) )
      return error;

    /* check whether we have a valid TrueType file */
    error = sfnt->load_font_dir( face, stream );
    if ( error )
      return error;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    {
      FT_ULong  fvar_len;

      FT_ULong  version;
      FT_ULong  offset;

      FT_UShort  num_axes;
      FT_UShort  axis_size;
      FT_UShort  num_instances;
      FT_UShort  instance_size;

      FT_Int  instance_index;

      FT_Byte*  default_values  = NULL;
      FT_Byte*  instance_values = NULL;


      face->is_default_instance = 1;

      instance_index = FT_ABS( face_instance_index ) >> 16;

      /* test whether current face is a GX font with named instances */
      if ( face->goto_table( face, TTAG_fvar, stream, &fvar_len ) ||
           fvar_len < 20                                          ||
           FT_READ_ULONG( version )                               ||
           FT_READ_USHORT( offset )                               ||
           FT_STREAM_SKIP( 2 ) /* reserved */                     ||
           FT_READ_USHORT( num_axes )                             ||
           FT_READ_USHORT( axis_size )                            ||
           FT_READ_USHORT( num_instances )                        ||
           FT_READ_USHORT( instance_size )                        )
      {
        version       = 0;
        offset        = 0;
        num_axes      = 0;
        axis_size     = 0;
        num_instances = 0;
        instance_size = 0;
      }

      /* check that the data is bound by the table length */
      if ( version != 0x00010000UL                    ||
           axis_size != 20                            ||
           num_axes == 0                              ||
           /* `num_axes' limit implied by 16-bit `instance_size' */
           num_axes > 0x3FFE                          ||
           !( instance_size == 4 + 4 * num_axes ||
              instance_size == 6 + 4 * num_axes )     ||
           /* `num_instances' limit implied by limited range of name IDs */
           num_instances > 0x7EFF                     ||
           offset                          +
             axis_size * num_axes          +
             instance_size * num_instances > fvar_len )
        num_instances = 0;
      else
        face->variation_support |= TT_FACE_FLAG_VAR_FVAR;

      /*
       *  As documented in the OpenType specification, an entry for the
       *  default instance may be omitted in the named instance table.  In
       *  particular this means that even if there is no named instance
       *  table in the font we actually do have a named instance, namely the
       *  default instance.
       *
       *  For consistency, we always want the default instance in our list
       *  of named instances.  If it is missing, we try to synthesize it
       *  later on.  Here, we have to adjust `num_instances' accordingly.
       */

      if ( !( FT_ALLOC( default_values, num_axes * 2 )  ||
              FT_ALLOC( instance_values, num_axes * 2 ) ) )
      {
        /* the current stream position is 16 bytes after the table start */
        FT_ULong  array_start = FT_STREAM_POS() - 16 + offset;
        FT_ULong  default_value_offset, instance_offset;

        FT_Byte*  p;
        FT_UInt   i;


        default_value_offset = array_start + 8;
        p                    = default_values;

        for ( i = 0; i < num_axes; i++ )
        {
          (void)FT_STREAM_READ_AT( default_value_offset, p, 2 );

          default_value_offset += axis_size;
          p                    += 2;
        }

        instance_offset = array_start + axis_size * num_axes + 4;

        for ( i = 0; i < num_instances; i++ )
        {
          (void)FT_STREAM_READ_AT( instance_offset,
                                   instance_values,
                                   num_axes * 2 );

          if ( !ft_memcmp( default_values, instance_values, num_axes * 2 ) )
            break;

          instance_offset += instance_size;
        }

        if ( i == num_instances )
        {
          /* no default instance in named instance table; */
          /* we thus have to synthesize it                */
          num_instances++;
        }
      }

       FT_FREE( default_values );
       FT_FREE( instance_values );
 
      /* we don't support Multiple Master CFFs yet; */
      /* note that `glyf' or `CFF2' have precedence */
       if ( face->goto_table( face, TTAG_glyf, stream, 0 ) &&
           face->goto_table( face, TTAG_CFF2, stream, 0 ) &&
            !face->goto_table( face, TTAG_CFF, stream, 0 ) )
         num_instances = 0;
      if ( instance_index > num_instances )
      {
        if ( face_instance_index >= 0 )
          return FT_THROW( Invalid_Argument );
        else
          num_instances = 0;
      }

      face->root.style_flags = (FT_Long)num_instances << 16;
    }
#endif

    face->root.num_faces  = face->ttc_header.count;
    face->root.face_index = face_instance_index;

    return error;
  }
