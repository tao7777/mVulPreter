  main( int    argc,
        char*  argv[] )
  {
    int    old_ptsize, orig_ptsize, file;
    int    first_glyph = 0;
    int    XisSetup = 0;
    char*  execname;
    int    option;
    int    file_loaded;

    grEvent  event;


    execname = ft_basename( argv[0] );

    while ( 1 )
    {
      option = getopt( argc, argv, "d:e:f:r:" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'd':
        parse_design_coords( optarg );
        break;

      case 'e':
        encoding = (FT_Encoding)make_tag( optarg );
        break;

      case 'f':
        first_glyph = atoi( optarg );
        break;

      case 'r':
        res = atoi( optarg );
        if ( res < 1 )
          usage( execname );
        break;

      default:
        usage( execname );
        break;
      }
    }

    argc -= optind;
    argv += optind;

    if ( argc <= 1 )
      usage( execname );

    if ( sscanf( argv[0], "%d", &orig_ptsize ) != 1 )
      orig_ptsize = 64;

    file = 1;

    /* Initialize engine */
    error = FT_Init_FreeType( &library );
    if ( error )
      PanicZ( "Could not initialize FreeType library" );

  NewFile:
    ptsize      = orig_ptsize;
    hinted      = 1;
    file_loaded = 0;

    /* Load face */
    error = FT_New_Face( library, argv[file], 0, &face );
    if ( error )
      goto Display_Font;

    if ( encoding != FT_ENCODING_NONE )
    {
      error = FT_Select_Charmap( face, encoding );
      if ( error )
        goto Display_Font;
    }

    /* retrieve multiple master information */
    error = FT_Get_MM_Var( face, &multimaster );
    if ( error )
      goto Display_Font;

    /* if the user specified a position, use it, otherwise */
    /* set the current position to the median of each axis */
    {
      int  n;


      for ( n = 0; n < (int)multimaster->num_axis; n++ )
      {
        design_pos[n] = n < requested_cnt ? requested_pos[n]
                                          : multimaster->axis[n].def;
        if ( design_pos[n] < multimaster->axis[n].minimum )
          design_pos[n] = multimaster->axis[n].minimum;
        else if ( design_pos[n] > multimaster->axis[n].maximum )
          design_pos[n] = multimaster->axis[n].maximum;
      }
    }

    error = FT_Set_Var_Design_Coordinates( face,
                                           multimaster->num_axis,
                                           design_pos );
    if ( error )
      goto Display_Font;

    file_loaded++;

    Reset_Scale( ptsize );

    num_glyphs = face->num_glyphs;
    glyph      = face->glyph;
    size       = face->size;

  Display_Font:
    /* initialize graphics if needed */
    if ( !XisSetup )
    {
      XisSetup = 1;
      Init_Display();
    }

    grSetTitle( surface, "FreeType Glyph Viewer - press F1 for help" );
    old_ptsize = ptsize;

    if ( file_loaded >= 1 )
    {
      Fail = 0;
      Num  = first_glyph;

      if ( Num >= num_glyphs )
        Num = num_glyphs - 1;

      if ( Num < 0 )
        Num = 0;
    }

    for ( ;; )
    {
      int  key;


      Clear_Display();

      if ( file_loaded >= 1 )
      {
        switch ( render_mode )
        {
        case 0:
          Render_Text( Num );
          break;

        default:
           Render_All( Num, ptsize );
         }
 
        sprintf( Header, "%.50s %.50s (file %.100s)",
                          face->family_name,
                          face->style_name,
                          ft_basename( argv[file] ) );

        if ( !new_header )
          new_header = Header;

        grWriteCellString( &bit, 0, 0, new_header, fore_color );
        new_header = 0;

        sprintf( Header, "axis: " );
        {
          int  n;


          for ( n = 0; n < (int)multimaster->num_axis; n++ )
          {
            char  temp[32];


            sprintf( temp, "  %s:%g",
                           multimaster->axis[n].name,
                           design_pos[n]/65536. );
            strcat( Header, temp );
          }
        }
        grWriteCellString( &bit, 0, 16, Header, fore_color );

        sprintf( Header, "at %d points, first glyph = %d",
                         ptsize,
                         Num );
       }
       else
       {
        sprintf( Header, "%.100s: not an MM font file, or could not be opened",
                          ft_basename( argv[file] ) );
       }
      grWriteCellString( &bit, 0, 8, Header, fore_color );
      grRefreshSurface( surface );

      grListenSurface( surface, 0, &event );
      if ( !( key = Process_Event( &event ) ) )
        goto End;

      if ( key == 'n' )
      {
        if ( file_loaded >= 1 )
          FT_Done_Face( face );

        if ( file < argc - 1 )
          file++;

        goto NewFile;
      }

      if ( key == 'p' )
      {
        if ( file_loaded >= 1 )
          FT_Done_Face( face );

        if ( file > 1 )
          file--;

        goto NewFile;
      }

      if ( ptsize != old_ptsize )
      {
        Reset_Scale( ptsize );

        old_ptsize = ptsize;
      }
    }

  End:
    grDoneSurface( surface );
    grDoneDevices();

    free            ( multimaster );
    FT_Done_Face    ( face        );
    FT_Done_FreeType( library     );

    printf( "Execution completed successfully.\n" );
    printf( "Fails = %d\n", Fail );

    exit( 0 );      /* for safety reasons */
    return 0;       /* never reached */
  }
