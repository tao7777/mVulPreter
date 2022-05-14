static cJSON *cJSON_New_Item( void )
static cJSON *cJSON_New_Item(void)
 {
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node) memset(node,0,sizeof(cJSON));
 	return node;
 }
