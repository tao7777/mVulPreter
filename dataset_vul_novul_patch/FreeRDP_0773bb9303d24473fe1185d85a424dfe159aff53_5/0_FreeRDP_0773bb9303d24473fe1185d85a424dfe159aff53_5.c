void* sspi_SecureHandleGetUpperPointer(SecHandle* handle)
 {
 	void* pointer;
 
	if (!handle || !SecIsValidHandle(handle))
 		return NULL;
 
 	pointer = (void*) ~((size_t) handle->dwUpper);

	return pointer;
}
