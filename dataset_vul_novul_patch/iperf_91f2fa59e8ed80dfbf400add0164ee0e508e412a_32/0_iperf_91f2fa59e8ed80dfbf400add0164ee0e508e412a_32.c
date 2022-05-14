static char* cJSON_strdup( const char* str )
void cJSON_InitHooks(cJSON_Hooks* hooks)
 {
    if (!hooks) { /* Reset hooks */
        cJSON_malloc = malloc;
        cJSON_free = free;
        return;
    }
 
	cJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
	cJSON_free	 = (hooks->free_fn)?hooks->free_fn:free;
 }
