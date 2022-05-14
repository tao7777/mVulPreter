void * CAPSTONE_API cs_winkernel_malloc(size_t size)
{
	NT_ASSERT(size);
 
 #pragma prefast(suppress : 30030)		// Allocating executable POOL_TYPE memory
	CS_WINKERNEL_MEMBLOCK *block = (CS_WINKERNEL_MEMBLOCK *)ExAllocatePoolWithTag(
			NonPagedPool, size + sizeof(CS_WINKERNEL_MEMBLOCK), CS_WINKERNEL_POOL_TAG);
 	if (!block) {
 		return NULL;
 	}
	block->size = size;

	return block->data;
}
