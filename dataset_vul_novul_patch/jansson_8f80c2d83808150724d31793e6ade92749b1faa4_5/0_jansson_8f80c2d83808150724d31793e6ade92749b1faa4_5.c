void *hashtable_iter_at(hashtable_t *hashtable, const char *key)
{
    pair_t *pair;
    size_t hash;
     bucket_t *bucket;
 
     hash = hash_str(key);
    bucket = &hashtable->buckets[hash & hashmask(hashtable->order)];
 
     pair = hashtable_find_pair(hashtable, bucket, key, hash);
     if(!pair)
        return NULL;

    return &pair->list;
}
