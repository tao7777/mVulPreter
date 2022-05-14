  T42_Face_Init( FT_Stream      stream,
                 FT_Face        t42face,       /* T42_Face */
                 FT_Int         face_index,
                 FT_Int         num_params,
                 FT_Parameter*  params )
  {
    T42_Face            face  = (T42_Face)t42face;
    FT_Error            error;
    FT_Service_PsCMaps  psnames;
    PSAux_Service       psaux;
    FT_Face             root  = (FT_Face)&face->root;
    T1_Font             type1 = &face->type1;
    PS_FontInfo         info  = &type1->font_info;

    FT_UNUSED( num_params );
    FT_UNUSED( params );
    FT_UNUSED( stream );


    face->ttf_face       = NULL;
    face->root.num_faces = 1;

    FT_FACE_FIND_GLOBAL_SERVICE( face, psnames, POSTSCRIPT_CMAPS );
    face->psnames = psnames;

    face->psaux = FT_Get_Module_Interface( FT_FACE_LIBRARY( face ),
                                           "psaux" );
    psaux = (PSAux_Service)face->psaux;
    if ( !psaux )
    {
      FT_ERROR(( "T42_Face_Init: cannot access `psaux' module\n" ));
      error = FT_THROW( Missing_Module );
      goto Exit;
    }

    FT_TRACE2(( "Type 42 driver\n" ));

    /* open the tokenizer, this will also check the font format */
    error = T42_Open_Face( face );
    if ( error )
      goto Exit;

    /* if we just wanted to check the format, leave successfully now */
    if ( face_index < 0 )
      goto Exit;

    /* check the face index */
    if ( face_index > 0 )
    {
      FT_ERROR(( "T42_Face_Init: invalid face index\n" ));
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    /* Now load the font program into the face object */

    /* Init the face object fields */
    /* Now set up root face fields */

    root->num_glyphs   = type1->num_glyphs;
    root->num_charmaps = 0;
    root->face_index   = 0;

    root->face_flags |= FT_FACE_FLAG_SCALABLE    |
                        FT_FACE_FLAG_HORIZONTAL  |
                        FT_FACE_FLAG_GLYPH_NAMES;

    if ( info->is_fixed_pitch )
      root->face_flags |= FT_FACE_FLAG_FIXED_WIDTH;

    /* We only set this flag if we have the patented bytecode interpreter. */
    /* There are no known `tricky' Type42 fonts that could be loaded with  */
    /* the unpatented interpreter.                                         */
#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER
    root->face_flags |= FT_FACE_FLAG_HINTER;
#endif

    /* XXX: TODO -- add kerning with .afm support */

    /* get style name -- be careful, some broken fonts only */
    /* have a `/FontName' dictionary entry!                 */
    root->family_name = info->family_name;
    /* assume "Regular" style if we don't know better */
    root->style_name = (char *)"Regular";
    if ( root->family_name )
    {
      char*  full   = info->full_name;
      char*  family = root->family_name;


      if ( full )
      {
        while ( *full )
        {
          if ( *full == *family )
          {
            family++;
            full++;
          }
          else
          {
            if ( *full == ' ' || *full == '-' )
              full++;
            else if ( *family == ' ' || *family == '-' )
              family++;
            else
            {
              if ( !*family )
                root->style_name = full;
              break;
            }
          }
        }
      }
    }
    else
    {
      /* do we have a `/FontName'? */
      if ( type1->font_name )
        root->family_name = type1->font_name;
    }

    /* no embedded bitmap support */
    root->num_fixed_sizes = 0;
    root->available_sizes = 0;

    /* Load the TTF font embedded in the T42 font */
    {
       FT_Open_Args  args;
 
 
      args.flags       = FT_OPEN_MEMORY | FT_OPEN_DRIVER;
      args.driver      = FT_Get_Module( FT_FACE_LIBRARY( face ),
                                        "truetype" );
       args.memory_base = face->ttf_data;
       args.memory_size = face->ttf_size;
        args.flags     |= FT_OPEN_PARAMS;
        args.num_params = num_params;
        args.params     = params;
      }

      error = FT_Open_Face( FT_FACE_LIBRARY( face ),
                            &args, 0, &face->ttf_face );
    }
