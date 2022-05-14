  Ins_GETVARIATION( TT_ExecContext  exc,
                    FT_Long*        args )
  {
    FT_UInt    num_axes = exc->face->blend->num_axis;
    FT_Fixed*  coords   = exc->face->blend->normalizedcoords;

    FT_UInt  i;


    if ( BOUNDS( num_axes, exc->stackSize + 1 - exc->top ) )
    {
      exc->error = FT_THROW( Stack_Overflow );
       return;
     }
 
    if ( coords )
    {
      for ( i = 0; i < num_axes; i++ )
        args[i] = coords[i] >> 2; /* convert 16.16 to 2.14 format */
    }
    else
    {
      for ( i = 0; i < num_axes; i++ )
        args[i] = 0;
    }
   }
