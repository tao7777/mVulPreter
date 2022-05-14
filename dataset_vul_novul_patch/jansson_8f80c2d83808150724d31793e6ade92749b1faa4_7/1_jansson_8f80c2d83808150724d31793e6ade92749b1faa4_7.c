static JSON_INLINE size_t num_buckets(hashtable_t *hashtable)
{
    return primes[hashtable->num_buckets];
}
