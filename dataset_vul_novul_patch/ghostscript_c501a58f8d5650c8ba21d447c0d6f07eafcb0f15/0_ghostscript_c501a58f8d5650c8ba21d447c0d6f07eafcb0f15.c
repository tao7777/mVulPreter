   static void  Ins_JMPR( INS_ARG )
   {
    if ( BOUNDS(CUR.IP + args[0], CUR.codeSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

     CUR.IP      += (Int)(args[0]);
     CUR.step_ins = FALSE;
     * allow for simple cases here by just checking the preceding byte.
     * Fonts with this problem are not uncommon.
     */
      CUR.IP -= 1;
  }
