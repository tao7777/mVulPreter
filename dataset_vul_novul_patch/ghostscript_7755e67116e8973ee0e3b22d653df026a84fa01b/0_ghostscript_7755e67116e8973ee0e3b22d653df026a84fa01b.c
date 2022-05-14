  static void  Ins_MDRP( INS_ARG )
  {
    Int         point;
    TT_F26Dot6  distance,
                org_dist;
 
     point = (Int)args[0];
 
    if ( BOUNDS( args[0], CUR.zp1.n_points ) ||
         BOUNDS( CUR.GS.rp0, CUR.zp0.n_points) )
     {
         /* Current version of FreeType silently ignores this out of bounds error
          * and drops the instruction, see bug #691121
      return;
    }

    /* XXX: Is there some undocumented feature while in the */
    /*      twilight zone?                                  */

    org_dist = CUR_Func_dualproj( CUR.zp1.org_x[point] -
                                    CUR.zp0.org_x[CUR.GS.rp0],
                                  CUR.zp1.org_y[point] -
                                    CUR.zp0.org_y[CUR.GS.rp0] );

    /* single width cutin test */

    if ( ABS(org_dist) < CUR.GS.single_width_cutin )
    {
      if ( org_dist >= 0 )
        org_dist = CUR.GS.single_width_value;
      else
        org_dist = -CUR.GS.single_width_value;
    }

    /* round flag */

    if ( (CUR.opcode & 4) != 0 )
      distance = CUR_Func_round( org_dist,
                                 CUR.metrics.compensations[CUR.opcode & 3] );
    else
      distance = Round_None( EXEC_ARGS
                             org_dist,
                             CUR.metrics.compensations[CUR.opcode & 3]  );

    /* minimum distance flag */

    if ( (CUR.opcode & 8) != 0 )
    {
      if ( org_dist >= 0 )
      {
        if ( distance < CUR.GS.minimum_distance )
          distance = CUR.GS.minimum_distance;
      }
      else
      {
        if ( distance > -CUR.GS.minimum_distance )
          distance = -CUR.GS.minimum_distance;
      }
    }

    /* now move the point */

    org_dist = CUR_Func_project( CUR.zp1.cur_x[point] -
                                   CUR.zp0.cur_x[CUR.GS.rp0],
                                 CUR.zp1.cur_y[point] -
                                   CUR.zp0.cur_y[CUR.GS.rp0] );

    CUR_Func_move( &CUR.zp1, point, distance - org_dist );

    CUR.GS.rp1 = CUR.GS.rp0;
    CUR.GS.rp2 = point;

    if ( (CUR.opcode & 16) != 0 )
      CUR.GS.rp0 = point;
  }
