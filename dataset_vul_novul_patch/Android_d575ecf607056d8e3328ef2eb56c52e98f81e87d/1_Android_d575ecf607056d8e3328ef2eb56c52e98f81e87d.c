OMX_ERRORTYPE  omx_video::allocate_input_buffer(
        OMX_IN OMX_HANDLETYPE            hComp,
        OMX_INOUT OMX_BUFFERHEADERTYPE** bufferHdr,
        OMX_IN OMX_U32                   port,
        OMX_IN OMX_PTR                   appData,
        OMX_IN OMX_U32                   bytes)
{
 (void)hComp, (void)port;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
 unsigned   i = 0;

    DEBUG_PRINT_HIGH("allocate_input_buffer()::");
 if (bytes != m_sInPortDef.nBufferSize) {
        DEBUG_PRINT_ERROR("ERROR: Buffer size mismatch error: bytes[%u] != nBufferSize[%u]",
 (unsigned int)bytes, (unsigned int)m_sInPortDef.nBufferSize);
 return OMX_ErrorBadParameter;
 }

 if (!m_inp_mem_ptr) {
        DEBUG_PRINT_HIGH("%s: size = %u, actual cnt %u", __FUNCTION__,
 (unsigned int)m_sInPortDef.nBufferSize, (unsigned int)m_sInPortDef.nBufferCountActual);
        m_inp_mem_ptr = (OMX_BUFFERHEADERTYPE*) \
                        calloc( (sizeof(OMX_BUFFERHEADERTYPE)), m_sInPortDef.nBufferCountActual);
 if (m_inp_mem_ptr == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_inp_mem_ptr");
 return OMX_ErrorInsufficientResources;
 }

        DEBUG_PRINT_LOW("Successfully allocated m_inp_mem_ptr = %p", m_inp_mem_ptr);
        m_pInput_pmem = (struct pmem *) calloc(sizeof (struct pmem), m_sInPortDef.nBufferCountActual);

 if (m_pInput_pmem == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pInput_pmem");
 return OMX_ErrorInsufficientResources;
 }
#ifdef USE_ION
        m_pInput_ion = (struct venc_ion *) calloc(sizeof (struct venc_ion), m_sInPortDef.nBufferCountActual);
 if (m_pInput_ion == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pInput_ion");
 return OMX_ErrorInsufficientResources;
 }
#endif
 for (i=0; i< m_sInPortDef.nBufferCountActual; i++) {
            m_pInput_pmem[i].fd = -1;
#ifdef USE_ION
            m_pInput_ion[i].ion_device_fd =-1;
            m_pInput_ion[i].fd_ion_data.fd =-1;
            m_pInput_ion[i].ion_alloc_data.handle = 0;
#endif
 }
 }

 for (i=0; i< m_sInPortDef.nBufferCountActual; i++) {
 if (BITMASK_ABSENT(&m_inp_bm_count,i)) {
 break;
 }
 }
 if (i < m_sInPortDef.nBufferCountActual) {

 *bufferHdr = (m_inp_mem_ptr + i);
 (*bufferHdr)->nSize             = sizeof(OMX_BUFFERHEADERTYPE);
 (*bufferHdr)->nVersion.nVersion = OMX_SPEC_VERSION;
 (*bufferHdr)->nAllocLen         = m_sInPortDef.nBufferSize;
 (*bufferHdr)->pAppPrivate       = appData;
 (*bufferHdr)->nInputPortIndex   = PORT_INDEX_IN;
 (*bufferHdr)->pInputPortPrivate = (OMX_PTR)&m_pInput_pmem[i];

#ifdef USE_ION
#ifdef _MSM8974_
        m_pInput_ion[i].ion_device_fd = alloc_map_ion_memory(m_sInPortDef.nBufferSize,
 &m_pInput_ion[i].ion_alloc_data,
 &m_pInput_ion[i].fd_ion_data,0);
#else
        m_pInput_ion[i].ion_device_fd = alloc_map_ion_memory(m_sInPortDef.nBufferSize,
 &m_pInput_ion[i].ion_alloc_data,
 &m_pInput_ion[i].fd_ion_data,ION_FLAG_CACHED);
#endif
 if (m_pInput_ion[i].ion_device_fd < 0) {
            DEBUG_PRINT_ERROR("ERROR:ION device open() Failed");
 return OMX_ErrorInsufficientResources;
 }

        m_pInput_pmem[i].fd = m_pInput_ion[i].fd_ion_data.fd;
#else
        m_pInput_pmem[i].fd = open (MEM_DEVICE,O_RDWR);

 if (m_pInput_pmem[i].fd == 0) {
            m_pInput_pmem[i].fd = open (MEM_DEVICE,O_RDWR);
 }

 if (m_pInput_pmem[i].fd < 0) {
            DEBUG_PRINT_ERROR("ERROR: /dev/pmem_adsp open() Failed");
 return OMX_ErrorInsufficientResources;
 }
#endif
        m_pInput_pmem[i].size = m_sInPortDef.nBufferSize;
        m_pInput_pmem[i].offset = 0;

        m_pInput_pmem[i].buffer = (OMX_U8 *)SECURE_BUFPTR;
 if(!secure_session) {
            m_pInput_pmem[i].buffer = (unsigned char *)mmap(NULL,
                m_pInput_pmem[i].size,PROT_READ|PROT_WRITE,
                MAP_SHARED,m_pInput_pmem[i].fd,0);
 if (m_pInput_pmem[i].buffer == MAP_FAILED) {
                DEBUG_PRINT_ERROR("ERROR: mmap FAILED= %d", errno);
            close(m_pInput_pmem[i].fd);
#ifdef USE_ION
            free_ion_memory(&m_pInput_ion[i]);
#endif
 return OMX_ErrorInsufficientResources;
 }
 } else {

             m_pInput_pmem[i].buffer = malloc(sizeof(OMX_U32) + sizeof(native_handle_t*));
         }
 
         (*bufferHdr)->pBuffer           = (OMX_U8 *)m_pInput_pmem[i].buffer;
        DEBUG_PRINT_LOW("Virtual address in allocate buffer is %p", m_pInput_pmem[i].buffer);
        BITMASK_SET(&m_inp_bm_count,i);
 if (!mUseProxyColorFormat && (dev_use_buf(&m_pInput_pmem[i],PORT_INDEX_IN,i) != true)) {
            DEBUG_PRINT_ERROR("ERROR: dev_use_buf FAILED for i/p buf");
 return OMX_ErrorInsufficientResources;
 }
 } else {
        DEBUG_PRINT_ERROR("ERROR: All i/p buffers are allocated, invalid allocate buf call"
 "for index [%d]", i);
        eRet = OMX_ErrorInsufficientResources;
 }

 return eRet;
}
