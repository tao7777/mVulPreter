OMX_ERRORTYPE  omx_video::allocate_output_buffer(
        OMX_IN OMX_HANDLETYPE            hComp,
        OMX_INOUT OMX_BUFFERHEADERTYPE** bufferHdr,
        OMX_IN OMX_U32                   port,
        OMX_IN OMX_PTR                   appData,
        OMX_IN OMX_U32                   bytes)
{
 (void)hComp, (void)port;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE       *bufHdr= NULL; // buffer header
 unsigned                         i= 0; // Temporary counter
#ifdef _MSM8974_
 int align_size;
#endif
    DEBUG_PRINT_HIGH("allocate_output_buffer()for %u bytes", (unsigned int)bytes);
 if (!m_out_mem_ptr) {
 int nBufHdrSize        = 0;
        DEBUG_PRINT_HIGH("%s: size = %u, actual cnt %u", __FUNCTION__,
 (unsigned int)m_sOutPortDef.nBufferSize, (unsigned int)m_sOutPortDef.nBufferCountActual);
        nBufHdrSize        = m_sOutPortDef.nBufferCountActual * sizeof(OMX_BUFFERHEADERTYPE);

 /*
         * Memory for output side involves the following:
         * 1. Array of Buffer Headers
         * 2. Bitmask array to hold the buffer allocation details
         * In order to minimize the memory management entire allocation
         * is done in one step.
         */
        m_out_mem_ptr = (OMX_BUFFERHEADERTYPE  *)calloc(nBufHdrSize,1);

#ifdef USE_ION
        m_pOutput_ion = (struct venc_ion *) calloc(sizeof (struct venc_ion), m_sOutPortDef.nBufferCountActual);
 if (m_pOutput_ion == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pOutput_ion");
 return OMX_ErrorInsufficientResources;
 }
#endif
        m_pOutput_pmem = (struct pmem *) calloc(sizeof(struct pmem), m_sOutPortDef.nBufferCountActual);
 if (m_pOutput_pmem == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pOutput_pmem");
 return OMX_ErrorInsufficientResources;
 }
 if (m_out_mem_ptr && m_pOutput_pmem) {
            bufHdr          =  m_out_mem_ptr;

 for (i=0; i < m_sOutPortDef.nBufferCountActual ; i++) {
                bufHdr->nSize              = sizeof(OMX_BUFFERHEADERTYPE);
                bufHdr->nVersion.nVersion  = OMX_SPEC_VERSION;
                bufHdr->nAllocLen          = bytes;
                bufHdr->nFilledLen         = 0;
                bufHdr->pAppPrivate        = appData;
                bufHdr->nOutputPortIndex   = PORT_INDEX_OUT;
                bufHdr->pOutputPortPrivate = (OMX_PTR)&m_pOutput_pmem[i];
                bufHdr->pBuffer            = NULL;
                bufHdr++;
                m_pOutput_pmem[i].fd = -1;
#ifdef USE_ION
                m_pOutput_ion[i].ion_device_fd =-1;
                m_pOutput_ion[i].fd_ion_data.fd=-1;
                m_pOutput_ion[i].ion_alloc_data.handle = 0;
#endif
 }
 } else {
            DEBUG_PRINT_ERROR("ERROR: calloc() failed for m_out_mem_ptr/m_pOutput_pmem");
            eRet = OMX_ErrorInsufficientResources;
 }
 }

    DEBUG_PRINT_HIGH("actual cnt = %u", (unsigned int)m_sOutPortDef.nBufferCountActual);
 for (i=0; i< m_sOutPortDef.nBufferCountActual; i++) {
 if (BITMASK_ABSENT(&m_out_bm_count,i)) {
            DEBUG_PRINT_LOW("Found a Free Output Buffer %d",i);
 break;
 }
 }
 if (eRet == OMX_ErrorNone) {
 if (i < m_sOutPortDef.nBufferCountActual) {
#ifdef USE_ION
#ifdef _MSM8974_
            align_size = ((m_sOutPortDef.nBufferSize + 4095)/4096) * 4096;
            m_pOutput_ion[i].ion_device_fd = alloc_map_ion_memory(align_size,
 &m_pOutput_ion[i].ion_alloc_data,
 &m_pOutput_ion[i].fd_ion_data, ION_FLAG_CACHED);
#else
            m_pOutput_ion[i].ion_device_fd = alloc_map_ion_memory(m_sOutPortDef.nBufferSize,
 &m_pOutput_ion[i].ion_alloc_data,
 &m_pOutput_ion[i].fd_ion_data,ION_FLAG_CACHED);
#endif
 if (m_pOutput_ion[i].ion_device_fd < 0) {
                DEBUG_PRINT_ERROR("ERROR:ION device open() Failed");
 return OMX_ErrorInsufficientResources;
 }

            m_pOutput_pmem[i].fd = m_pOutput_ion[i].fd_ion_data.fd;
#else
            m_pOutput_pmem[i].fd = open (MEM_DEVICE,O_RDWR);
 if (m_pOutput_pmem[i].fd == 0) {
                m_pOutput_pmem[i].fd = open (MEM_DEVICE,O_RDWR);
 }

 if (m_pOutput_pmem[i].fd < 0) {
                DEBUG_PRINT_ERROR("ERROR: /dev/pmem_adsp open() failed");
 return OMX_ErrorInsufficientResources;
 }
#endif

             m_pOutput_pmem[i].size = m_sOutPortDef.nBufferSize;
             m_pOutput_pmem[i].offset = 0;
 
            m_pOutput_pmem[i].buffer = NULL;
             if(!secure_session) {
 #ifdef _MSM8974_
                 m_pOutput_pmem[i].buffer = (unsigned char *)mmap(NULL,
                    align_size,PROT_READ|PROT_WRITE,
                    MAP_SHARED,m_pOutput_pmem[i].fd,0);
#else
                m_pOutput_pmem[i].buffer = (unsigned char *)mmap(NULL,
                    m_pOutput_pmem[i].size,PROT_READ|PROT_WRITE,
                    MAP_SHARED,m_pOutput_pmem[i].fd,0);

 #endif
             if (m_pOutput_pmem[i].buffer == MAP_FAILED) {
                     DEBUG_PRINT_ERROR("ERROR: MMAP_FAILED in o/p alloc buffer");
                m_pOutput_pmem[i].buffer = NULL;
                 close (m_pOutput_pmem[i].fd);
 #ifdef USE_ION
                 free_ion_memory(&m_pOutput_ion[i]);
#endif
 return OMX_ErrorInsufficientResources;
 }
 }
 else {

                 m_pOutput_pmem[i].buffer = malloc(sizeof(OMX_U32) + sizeof(native_handle_t*));
                if (m_pOutput_pmem[i].buffer == NULL) {
                    DEBUG_PRINT_ERROR("%s: Failed to allocate native-handle", __func__);
                    return OMX_ErrorInsufficientResources;
                }
                 (*bufferHdr)->nAllocLen = sizeof(OMX_U32) + sizeof(native_handle_t*);
                 native_handle_t *handle = native_handle_create(1, 0);
                 handle->data[0] = m_pOutput_pmem[i].fd;
 char *data = (char*) m_pOutput_pmem[i].buffer;
                OMX_U32 type = 1;
                memcpy(data, &type, sizeof(OMX_U32));
                memcpy(data + sizeof(OMX_U32), &handle, sizeof(native_handle_t*));
 }

 *bufferHdr = (m_out_mem_ptr + i );
 (*bufferHdr)->pBuffer = (OMX_U8 *)m_pOutput_pmem[i].buffer;
 (*bufferHdr)->pAppPrivate = appData;

            BITMASK_SET(&m_out_bm_count,i);

 if (dev_use_buf(&m_pOutput_pmem[i],PORT_INDEX_OUT,i) != true) {
                DEBUG_PRINT_ERROR("ERROR: dev_use_buf FAILED for o/p buf");
 return OMX_ErrorInsufficientResources;
 }
 } else {
            DEBUG_PRINT_ERROR("ERROR: All o/p buffers are allocated, invalid allocate buf call"
 "for index [%d] actual: %u", i, (unsigned int)m_sOutPortDef.nBufferCountActual);
 }
 }

 return eRet;
}
