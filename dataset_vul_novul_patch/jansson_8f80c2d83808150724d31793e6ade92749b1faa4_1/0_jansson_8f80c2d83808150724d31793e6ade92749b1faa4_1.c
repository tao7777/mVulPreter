static int hashtable_do_del(hashtable_t *hashtable,
                            const char *key, size_t hash)
{
    pair_t *pair;
     bucket_t *bucket;
     size_t index;
 
    index = hash & hashmask(hashtable->order);
     bucket = &hashtable->buckets[index];
 
     pair = hashtable_find_pair(hashtable, bucket, key, hash);
    if(!pair)
        return -1;

    if(&pair->list == bucket->first && &pair->list == bucket->last)
        bucket->first = bucket->last = &hashtable->list;

    else if(&pair->list == bucket->first)
        bucket->first = pair->list.next;

    else if(&pair->list == bucket->last)
        bucket->last = pair->list.prev;

    list_remove(&pair->list);
    json_decref(pair->value);

    jsonp_free(pair);
    hashtable->size--;

    return 0;
}
