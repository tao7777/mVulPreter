  Horizontal_Sweep_Span( RAS_ARGS Short       y,
                                  FT_F26Dot6  x1,
                                  FT_F26Dot6  x2,
                                  PProfile    left,
                                  PProfile    right )
  {
    FT_UNUSED( left );
    FT_UNUSED( right );


    if ( x2 - x1 < ras.precision )
    {
      Long  e1, e2;


      e1 = CEILING( x1 );
      e2 = FLOOR  ( x2 );

      if ( e1 == e2 )
      {
        Byte   f1;
        PByte  bits;


        bits = ras.bTarget + ( y >> 3 );
        f1   = (Byte)( 0x80 >> ( y & 7 ) );
 
         e1 = TRUNC( e1 );
 
        if ( e1 >= 0 && (ULong)e1 < ras.target.rows )
         {
           PByte  p;
 

          p = bits - e1 * ras.target.pitch;
          if ( ras.target.pitch > 0 )
            p += ( ras.target.rows - 1 ) * ras.target.pitch;

          p[0] |= f1;
        }
      }
    }
  }
