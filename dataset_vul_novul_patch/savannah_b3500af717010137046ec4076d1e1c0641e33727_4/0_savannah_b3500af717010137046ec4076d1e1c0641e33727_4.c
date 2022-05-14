  Horizontal_Sweep_Drop( RAS_ARGS Short       y,
                                  FT_F26Dot6  x1,
                                  FT_F26Dot6  x2,
                                  PProfile    left,
                                  PProfile    right )
  {
    Long   e1, e2, pxl;
    PByte  bits;
    Byte   f1;


    /* During the horizontal sweep, we only take care of drop-outs */

    /* e1     +       <-- pixel center */
    /*        |                        */
    /* x1  ---+-->    <-- contour      */
    /*        |                        */
    /*        |                        */
    /* x2  <--+---    <-- contour      */
    /*        |                        */
    /*        |                        */
    /* e2     +       <-- pixel center */

    e1  = CEILING( x1 );
    e2  = FLOOR  ( x2 );
    pxl = e1;

    if ( e1 > e2 )
    {
      Int  dropOutControl = left->flags & 7;


      if ( e1 == e2 + ras.precision )
      {
        switch ( dropOutControl )
        {
        case 0: /* simple drop-outs including stubs */
          pxl = e2;
          break;

        case 4: /* smart drop-outs including stubs */
          pxl = FLOOR( ( x1 + x2 - 1 ) / 2 + ras.precision_half );
          break;

        case 1: /* simple drop-outs excluding stubs */
        case 5: /* smart drop-outs excluding stubs  */
          /* see Vertical_Sweep_Drop for details */

          /* rightmost stub test */
          if ( left->next == right                &&
               left->height <= 0                  &&
               !( left->flags & Overshoot_Top   &&
                  x2 - x1 >= ras.precision_half ) )
            return;

          /* leftmost stub test */
          if ( right->next == left                 &&
               left->start == y                    &&
               !( left->flags & Overshoot_Bottom &&
                  x2 - x1 >= ras.precision_half  ) )
            return;

          if ( dropOutControl == 1 )
            pxl = e2;
          else
            pxl = FLOOR( ( x1 + x2 - 1 ) / 2 + ras.precision_half );
          break;

        default: /* modes 2, 3, 6, 7 */
          return;  /* no drop-out control */
        }

        /* undocumented but confirmed: If the drop-out would result in a  */
        /* pixel outside of the bounding box, use the pixel inside of the */
         /* bounding box instead                                           */
         if ( pxl < 0 )
           pxl = e1;
        else if ( (ULong)( TRUNC( pxl ) ) >= ras.target.rows )
           pxl = e2;
 
         /* check that the other pixel isn't set */
        e1 = pxl == e1 ? e2 : e1;

        e1 = TRUNC( e1 );

        bits = ras.bTarget + ( y >> 3 );
        f1   = (Byte)( 0x80 >> ( y & 7 ) );

        bits -= e1 * ras.target.pitch;
         if ( ras.target.pitch > 0 )
           bits += ( ras.target.rows - 1 ) * ras.target.pitch;
 
        if ( e1 >= 0                     &&
             (ULong)e1 < ras.target.rows &&
             *bits & f1                  )
           return;
       }
       else
        return;
    }

    bits = ras.bTarget + ( y >> 3 );
    f1   = (Byte)( 0x80 >> ( y & 7 ) );
 
     e1 = TRUNC( pxl );
 
    if ( e1 >= 0 && (ULong)e1 < ras.target.rows )
     {
       bits -= e1 * ras.target.pitch;
       if ( ras.target.pitch > 0 )
        bits += ( ras.target.rows - 1 ) * ras.target.pitch;

      bits[0] |= f1;
    }
  }
