static int hashtable_do_rehash(hashtable_t *hashtable)
{
    list_t *list, *next;
    pair_t *pair;
    size_t i, index, new_size;
 
     jsonp_free(hashtable->buckets);
 
    hashtable->order++;
    new_size = hashsize(hashtable->order);
 
     hashtable->buckets = jsonp_malloc(new_size * sizeof(bucket_t));
     if(!hashtable->buckets)
         return -1;
 
    for(i = 0; i < hashsize(hashtable->order); i++)
     {
         hashtable->buckets[i].first = hashtable->buckets[i].last =
             &hashtable->list;
    }

    list = hashtable->list.next;
    list_init(&hashtable->list);

    for(; list != &hashtable->list; list = next) {
        next = list->next;
        pair = list_to_pair(list);
        index = pair->hash % new_size;
        insert_to_bucket(hashtable, &hashtable->buckets[index], &pair->list);
    }

    return 0;
}
