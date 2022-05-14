  Ins_IUP( INS_ARG )
  {
    IUP_WorkerRec  V;
    FT_Byte        mask;

    FT_UInt   first_point;   /* first point of contour        */
    FT_UInt   end_point;     /* end point (last+1) of contour */

    FT_UInt   first_touched; /* first touched point in contour   */
    FT_UInt   cur_touched;   /* current touched point in contour */

    FT_UInt   point;         /* current point   */
    FT_Short  contour;       /* current contour */

    FT_UNUSED_ARG;


    /* ignore empty outlines */
    if ( CUR.pts.n_contours == 0 )
      return;

    if ( CUR.opcode & 1 )
    {
      mask   = FT_CURVE_TAG_TOUCH_X;
      V.orgs = CUR.pts.org;
      V.curs = CUR.pts.cur;
      V.orus = CUR.pts.orus;
    }
    else
    {
      mask   = FT_CURVE_TAG_TOUCH_Y;
      V.orgs = (FT_Vector*)( (FT_Pos*)CUR.pts.org + 1 );
      V.curs = (FT_Vector*)( (FT_Pos*)CUR.pts.cur + 1 );
      V.orus = (FT_Vector*)( (FT_Pos*)CUR.pts.orus + 1 );
    }
    V.max_points = CUR.pts.n_points;

    contour = 0;
    point   = 0;

    do
    {
       end_point   = CUR.pts.contours[contour] - CUR.pts.first_point;
       first_point = point;
 
      if ( BOUNDS ( end_point, CUR.pts.n_points ) )
        end_point = CUR.pts.n_points - 1;
 
       while ( point <= end_point && ( CUR.pts.tags[point] & mask ) == 0 )
         point++;

      if ( point <= end_point )
      {
        first_touched = point;
        cur_touched   = point;

        point++;

        while ( point <= end_point )
        {
          if ( ( CUR.pts.tags[point] & mask ) != 0 )
          {
            if ( point > 0 )
              _iup_worker_interpolate( &V,
                                       cur_touched + 1,
                                       point - 1,
                                       cur_touched,
                                       point );
            cur_touched = point;
          }

          point++;
        }

        if ( cur_touched == first_touched )
          _iup_worker_shift( &V, first_point, end_point, cur_touched );
        else
        {
          _iup_worker_interpolate( &V,
                                   (FT_UShort)( cur_touched + 1 ),
                                   end_point,
                                   cur_touched,
                                   first_touched );

          if ( first_touched > 0 )
            _iup_worker_interpolate( &V,
                                     first_point,
                                     first_touched - 1,
                                     cur_touched,
                                     first_touched );
        }
      }
      contour++;
    } while ( contour < CUR.pts.n_contours );
  }
