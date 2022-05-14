   static void  Ins_JMPR( INS_ARG )
   {
     CUR.IP      += (Int)(args[0]);
     CUR.step_ins = FALSE;
     * allow for simple cases here by just checking the preceding byte.
     * Fonts with this problem are not uncommon.
     */
      CUR.IP -= 1;
  }
