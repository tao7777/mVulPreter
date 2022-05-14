void hashtable_clear(hashtable_t *hashtable)
{
    size_t i;
 
     hashtable_do_clear(hashtable);
 
    for(i = 0; i < hashsize(hashtable->order); i++)
     {
         hashtable->buckets[i].first = hashtable->buckets[i].last =
             &hashtable->list;
    }

    list_init(&hashtable->list);
    hashtable->size = 0;
}
