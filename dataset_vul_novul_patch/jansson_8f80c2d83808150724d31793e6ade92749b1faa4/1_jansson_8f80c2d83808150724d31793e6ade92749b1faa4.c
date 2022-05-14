static size_t hash_str(const void *ptr)
{
    const char *str = (const char *)ptr;
    size_t hash = 5381;
    size_t c;
 
    while((c = (size_t)*str))
    {
        hash = ((hash << 5) + hash) + c;
        str++;
    }
 
    return hash;
}
