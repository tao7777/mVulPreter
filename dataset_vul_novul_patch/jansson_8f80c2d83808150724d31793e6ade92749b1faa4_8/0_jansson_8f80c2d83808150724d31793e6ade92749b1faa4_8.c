 json_t *json_object(void)
 {
     json_object_t *object = jsonp_malloc(sizeof(json_object_t));
     if(!object)
         return NULL;

    if (!hashtable_seed) {
        /* Autoseed */
        json_object_seed(0);
    }

     json_init(&object->json, JSON_OBJECT);
 
     if(hashtable_init(&object->hashtable))
    {
        jsonp_free(object);
        return NULL;
    }

    object->serial = 0;
    object->visited = 0;

    return &object->json;
}
