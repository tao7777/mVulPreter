static void spl_ptr_heap_insert(spl_ptr_heap *heap, spl_ptr_heap_element elem, void *cmp_userdata TSRMLS_DC) { /* {{{ */
	int i;

	if (heap->count+1 > heap->max_size) {
		/* we need to allocate more memory */
		heap->elements  = (void **) safe_erealloc(heap->elements, sizeof(spl_ptr_heap_element), (heap->max_size), (sizeof(spl_ptr_heap_element) * (heap->max_size)));
		heap->max_size *= 2;
	}

        heap->ctor(elem TSRMLS_CC);
 
        /* sifting up */
       for(i = heap->count; i > 0 && heap->cmp(heap->elements[(i-1)/2], elem, cmp_userdata TSRMLS_CC) < 0; i = (i-1)/2) {
                heap->elements[i] = heap->elements[(i-1)/2];
        }
       heap->count++;
 
        if (EG(exception)) {
                /* exception thrown during comparison */
	}

	heap->elements[i] = elem;

}
/* }}} */
