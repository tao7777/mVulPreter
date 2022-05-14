  cid_parse_font_matrix( CID_Face     face,
                         CID_Parser*  parser )
  {
    CID_FaceDict  dict;
    FT_Face       root = (FT_Face)&face->root;
    FT_Fixed      temp[6];
    FT_Fixed      temp_scale;


    if ( parser->num_dict >= 0 && parser->num_dict < face->cid.num_dicts )
     {
       FT_Matrix*  matrix;
       FT_Vector*  offset;
 
 
       dict   = face->cid.font_dicts + parser->num_dict;
       matrix = &dict->font_matrix;
       offset = &dict->font_offset;
 
      (void)cid_parser_to_fixed_array( parser, 6, temp, 3 );
 
       temp_scale = FT_ABS( temp[3] );
 
       /* Set Units per EM based on FontMatrix values.  We set the value to */
       /* 1000 / temp_scale, because temp_scale was already multiplied by   */
       /* 1000 (in t1_tofixed, from psobjs.c).                              */
        temp[5] = FT_DivFix( temp[5], temp_scale );
        temp[3] = 0x10000L;
      }

      matrix->xx = temp[0];
      matrix->yx = temp[1];
      matrix->xy = temp[2];
      matrix->yy = temp[3];
         temp[2] = FT_DivFix( temp[2], temp_scale );
         temp[4] = FT_DivFix( temp[4], temp_scale );
         temp[5] = FT_DivFix( temp[5], temp_scale );
        temp[3] = 0x10000L;
       }
