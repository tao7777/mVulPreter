static const char *parse_array( cJSON *item, const char *value )
static const char *parse_array(cJSON *item,const char *value,const char **ep)
 {
 	cJSON *child;
	if (*value!='[')	{*ep=value;return 0;}	/* not an array! */
 
	item->type=cJSON_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */
 
	item->child=child=cJSON_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value),ep));	/* skip any spacing, get the value. */
	if (!value) return 0;
 
	while (*value==',')
	{
 		cJSON *new_item;
		if (!(new_item=cJSON_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1),ep));
		if (!value) return 0;	/* memory fail */
 	}
 
	if (*value==']') return value+1;	/* end of array */
	*ep=value;return 0;	/* malformed. */
 }
