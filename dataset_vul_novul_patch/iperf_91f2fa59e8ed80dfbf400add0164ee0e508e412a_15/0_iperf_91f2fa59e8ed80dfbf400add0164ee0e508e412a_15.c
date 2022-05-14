void cJSON_Delete( cJSON *c )
void cJSON_Delete(cJSON *c)
 {
 	cJSON *next;
	while (c)
	{
		next=c->next;
		if (!(c->type&cJSON_IsReference) && c->child) cJSON_Delete(c->child);
		if (!(c->type&cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
		if (!(c->type&cJSON_StringIsConst) && c->string) cJSON_free(c->string);
		cJSON_free(c);
		c=next;
 	}
 }
