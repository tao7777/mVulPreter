  ftc_snode_load( FTC_SNode    snode,
                  FTC_Manager  manager,
                  FT_UInt      gindex,
                  FT_ULong    *asize )
  {
    FT_Error          error;
    FTC_GNode         gnode  = FTC_GNODE( snode );
    FTC_Family        family = gnode->family;
    FT_Memory         memory = manager->memory;
    FT_Face           face;
    FTC_SBit          sbit;
    FTC_SFamilyClass  clazz;


    if ( (FT_UInt)(gindex - gnode->gindex) >= snode->count )
    {
      FT_ERROR(( "ftc_snode_load: invalid glyph index" ));
      return FT_THROW( Invalid_Argument );
    }

    sbit  = snode->sbits + ( gindex - gnode->gindex );
    clazz = (FTC_SFamilyClass)family->clazz;

    sbit->buffer = 0;

    error = clazz->family_load_glyph( family, gindex, manager, &face );
    if ( error )
      goto BadGlyph;

    {
      FT_Int        temp;
      FT_GlyphSlot  slot   = face->glyph;
      FT_Bitmap*    bitmap = &slot->bitmap;
      FT_Pos        xadvance, yadvance; /* FT_GlyphSlot->advance.{x|y} */


      if ( slot->format != FT_GLYPH_FORMAT_BITMAP )
      {
        FT_TRACE0(( "ftc_snode_load:"
                    " glyph loaded didn't return a bitmap\n" ));
         goto BadGlyph;
       }
 
      /* Check whether our values fit into 8-bit containers!    */
       /* If this is not the case, our bitmap is too large       */
       /* and we will leave it as `missing' with sbit.buffer = 0 */
 
#define CHECK_CHAR( d )  ( temp = (FT_Char)d, (FT_Int) temp == (FT_Int) d )
#define CHECK_BYTE( d )  ( temp = (FT_Byte)d, (FT_UInt)temp == (FT_UInt)d )
 
       /* horizontal advance in pixels */
       xadvance = ( slot->advance.x + 32 ) >> 6;
      yadvance = ( slot->advance.y + 32 ) >> 6;

      if ( !CHECK_BYTE( bitmap->rows  )     ||
           !CHECK_BYTE( bitmap->width )     ||
           !CHECK_CHAR( bitmap->pitch )     ||
           !CHECK_CHAR( slot->bitmap_left ) ||
           !CHECK_CHAR( slot->bitmap_top  ) ||
           !CHECK_CHAR( xadvance )          ||
           !CHECK_CHAR( yadvance )          )
      {
        FT_TRACE2(( "ftc_snode_load:"
                    " glyph too large for small bitmap cache\n"));
        goto BadGlyph;
      }

      sbit->width     = (FT_Byte)bitmap->width;
      sbit->height    = (FT_Byte)bitmap->rows;
      sbit->pitch     = (FT_Char)bitmap->pitch;
      sbit->left      = (FT_Char)slot->bitmap_left;
      sbit->top       = (FT_Char)slot->bitmap_top;
      sbit->xadvance  = (FT_Char)xadvance;
      sbit->yadvance  = (FT_Char)yadvance;
      sbit->format    = (FT_Byte)bitmap->pixel_mode;
      sbit->max_grays = (FT_Byte)(bitmap->num_grays - 1);

      /* copy the bitmap into a new buffer -- ignore error */
      error = ftc_sbit_copy_bitmap( sbit, bitmap, memory );

      /* now, compute size */
      if ( asize )
        *asize = FT_ABS( sbit->pitch ) * sbit->height;

    } /* glyph loading successful */

    /* ignore the errors that might have occurred --   */
    /* we mark unloaded glyphs with `sbit.buffer == 0' */
    /* and `width == 255', `height == 0'               */
    /*                                                 */
    if ( error && FT_ERR_NEQ( error, Out_Of_Memory ) )
    {
    BadGlyph:
      sbit->width  = 255;
      sbit->height = 0;
      sbit->buffer = NULL;
      error        = FT_Err_Ok;
      if ( asize )
        *asize = 0;
    }

    return error;
  }
