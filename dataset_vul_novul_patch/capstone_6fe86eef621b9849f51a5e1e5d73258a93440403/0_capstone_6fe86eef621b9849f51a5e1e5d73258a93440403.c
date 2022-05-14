void * CAPSTONE_API cs_winkernel_malloc(size_t size)
{
	NT_ASSERT(size);
 
 #pragma prefast(suppress : 30030)		// Allocating executable POOL_TYPE memory
	size_t number_of_bytes = 0;
	CS_WINKERNEL_MEMBLOCK *block = NULL;
	// A specially crafted size value can trigger the overflow.
	// If the sum in a value that overflows or underflows the capacity of the type,
	// the function returns NULL.
	if (!NT_SUCCESS(RtlSizeTAdd(size, sizeof(CS_WINKERNEL_MEMBLOCK), &number_of_bytes))) {
		return NULL;
	}
	block = (CS_WINKERNEL_MEMBLOCK *)ExAllocatePoolWithTag(
			NonPagedPool, number_of_bytes, CS_WINKERNEL_POOL_TAG);
 	if (!block) {
 		return NULL;
 	}
	block->size = size;

	return block->data;
}
