 findoprnd(ITEM *ptr, int32 *pos)
 {
	/* since this function recurses, it could be driven to stack overflow. */
	check_stack_depth();

 #ifdef BS_DEBUG
 	elog(DEBUG3, (ptr[*pos].type == OPR) ?
 		 "%d  %c" : "%d  %d", *pos, ptr[*pos].val);
#endif
	if (ptr[*pos].type == VAL)
	{
		ptr[*pos].left = 0;
		(*pos)--;
	}
	else if (ptr[*pos].val == (int32) '!')
	{
		ptr[*pos].left = -1;
		(*pos)--;
		findoprnd(ptr, pos);
	}
	else
	{
		ITEM	   *curitem = &ptr[*pos];
		int32		tmp = *pos;

		(*pos)--;
		findoprnd(ptr, pos);
		curitem->left = *pos - tmp;
		findoprnd(ptr, pos);
	}
}
