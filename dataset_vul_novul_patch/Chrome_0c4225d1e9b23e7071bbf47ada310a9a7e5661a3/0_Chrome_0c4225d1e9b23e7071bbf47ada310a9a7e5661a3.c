bool JSArray::increaseVectorPrefixLength(unsigned newLength)
{
    
    ArrayStorage* storage = m_storage;
    
    unsigned vectorLength = m_vectorLength;
    ASSERT(newLength > vectorLength);
    ASSERT(newLength <= MAX_STORAGE_VECTOR_INDEX);
    unsigned newVectorLength = getNewVectorLength(newLength);

    void* newBaseStorage = fastMalloc(storageSize(newVectorLength + m_indexBias));
    if (!newBaseStorage)
        return false;
    
    m_indexBias += newVectorLength - newLength;
    
    m_storage = reinterpret_cast_ptr<ArrayStorage*>(static_cast<char*>(newBaseStorage) + m_indexBias * sizeof(JSValue));

    memcpy(m_storage, storage, storageSize(0));
    memcpy(&m_storage->m_vector[newLength - m_vectorLength], &storage->m_vector[0], vectorLength * sizeof(JSValue));
    
    m_storage->m_allocBase = newBaseStorage;
     m_vectorLength = newLength;
     
     fastFree(storage->m_allocBase);
    ASSERT(newLength > vectorLength);
    unsigned delta = newLength - vectorLength;
    for (unsigned i = 0; i < delta; i++)
        m_storage->m_vector[i].clear();
     Heap::heap(this)->reportExtraMemoryCost(storageSize(newVectorLength) - storageSize(vectorLength));
     
     return true;
}
