static HB_Error  Lookup_MarkMarkPos( GPOS_Instance*    gpi,
				     HB_GPOS_SubTable* st,
				     HB_Buffer        buffer,
				     HB_UShort         flags,
				     HB_UShort         context_length,
				     int               nesting_level )
{
  HB_UShort        i, j, mark1_index, mark2_index, property, class;
  HB_Fixed           x_mark1_value, y_mark1_value,
		   x_mark2_value, y_mark2_value;
  HB_Error         error;
  HB_GPOSHeader*  gpos = gpi->gpos;
  HB_MarkMarkPos* mmp = &st->markmark;

  HB_MarkArray*    ma1;
  HB_Mark2Array*   ma2;
  HB_Mark2Record*  m2r;
  HB_Anchor*       mark1_anchor;
  HB_Anchor*       mark2_anchor;

  HB_Position    o;

  HB_UNUSED(nesting_level);

  if ( context_length != 0xFFFF && context_length < 1 )
    return HB_Err_Not_Covered;

  if ( flags & HB_LOOKUP_FLAG_IGNORE_MARKS )
    return HB_Err_Not_Covered;

  if ( CHECK_Property( gpos->gdef, IN_CURITEM(),
		       flags, &property ) )
    return error;

  error = _HB_OPEN_Coverage_Index( &mmp->Mark1Coverage, IN_CURGLYPH(),
			  &mark1_index );
  if ( error )
    return error;

  /* now we search backwards for a suitable mark glyph until a non-mark
     glyph                                                */

  if ( buffer->in_pos == 0 )
    return HB_Err_Not_Covered;

  i = 1;
  j = buffer->in_pos - 1;
  while ( i <= buffer->in_pos )
  {
    error = HB_GDEF_Get_Glyph_Property( gpos->gdef, IN_GLYPH( j ),
					&property );
    if ( error )
      return error;

    if ( !( property == HB_GDEF_MARK || property & HB_LOOKUP_FLAG_IGNORE_SPECIAL_MARKS ) )
      return HB_Err_Not_Covered;

    if ( flags & HB_LOOKUP_FLAG_IGNORE_SPECIAL_MARKS )
    {
      if ( property == (flags & 0xFF00) )
        break;
    }
    else
      break;

    i++;
     j--;
   }
 
  if ( i > buffer->in_pos )
    return HB_Err_Not_Covered;

   error = _HB_OPEN_Coverage_Index( &mmp->Mark2Coverage, IN_GLYPH( j ),
 			  &mark2_index );
   if ( error )

  if ( mark1_index >= ma1->MarkCount )
    return ERR(HB_Err_Invalid_SubTable);

  class        = ma1->MarkRecord[mark1_index].Class;
  mark1_anchor = &ma1->MarkRecord[mark1_index].MarkAnchor;

  if ( class >= mmp->ClassCount )
    return ERR(HB_Err_Invalid_SubTable);

  ma2 = &mmp->Mark2Array;

  if ( mark2_index >= ma2->Mark2Count )
    return ERR(HB_Err_Invalid_SubTable);

  m2r          = &ma2->Mark2Record[mark2_index];
  mark2_anchor = &m2r->Mark2Anchor[class];

  error = Get_Anchor( gpi, mark1_anchor, IN_CURGLYPH(),
		      &x_mark1_value, &y_mark1_value );
  if ( error )
    return error;
  error = Get_Anchor( gpi, mark2_anchor, IN_GLYPH( j ),
		      &x_mark2_value, &y_mark2_value );
  if ( error )
    return error;

  /* anchor points are not cumulative */

  o = POSITION( buffer->in_pos );

  o->x_pos     = x_mark2_value - x_mark1_value;
  o->y_pos     = y_mark2_value - y_mark1_value;
  o->x_advance = 0;
  o->y_advance = 0;
  o->back      = 1;

  (buffer->in_pos)++;

  return HB_Err_Ok;
}
