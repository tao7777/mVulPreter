int hashtable_init(hashtable_t *hashtable)
{
     size_t i;
 
     hashtable->size = 0;
    hashtable->num_buckets = 0;  /* index to primes[] */
    hashtable->buckets = jsonp_malloc(num_buckets(hashtable) * sizeof(bucket_t));
     if(!hashtable->buckets)
         return -1;
 
     list_init(&hashtable->list);
 
    for(i = 0; i < num_buckets(hashtable); i++)
     {
         hashtable->buckets[i].first = hashtable->buckets[i].last =
             &hashtable->list;
    }

    return 0;
}
