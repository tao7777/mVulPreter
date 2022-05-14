OMX_ERRORTYPE  omx_video::use_output_buffer(
        OMX_IN OMX_HANDLETYPE            hComp,
        OMX_INOUT OMX_BUFFERHEADERTYPE** bufferHdr,
        OMX_IN OMX_U32                   port,
        OMX_IN OMX_PTR                   appData,
        OMX_IN OMX_U32                   bytes,
        OMX_IN OMX_U8*                   buffer)
{
 (void)hComp, (void)port;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE       *bufHdr= NULL; // buffer header
 unsigned                         i= 0; // Temporary counter
 unsigned char *buf_addr = NULL;
#ifdef _MSM8974_
 int align_size;
#endif

    DEBUG_PRINT_HIGH("Inside use_output_buffer()");
 if (bytes != m_sOutPortDef.nBufferSize) {
        DEBUG_PRINT_ERROR("ERROR: use_output_buffer: Size Mismatch!! "
 "bytes[%u] != Port.nBufferSize[%u]", (unsigned int)bytes, (unsigned int)m_sOutPortDef.nBufferSize);
 return OMX_ErrorBadParameter;
 }

 if (!m_out_mem_ptr) {
        output_use_buffer = true;
 int nBufHdrSize        = 0;

        DEBUG_PRINT_LOW("Allocating First Output Buffer(%u)",(unsigned int)m_sOutPortDef.nBufferCountActual);
        nBufHdrSize        = m_sOutPortDef.nBufferCountActual * sizeof(OMX_BUFFERHEADERTYPE);
 /*
         * Memory for output side involves the following:
         * 1. Array of Buffer Headers
         * 2. Bitmask array to hold the buffer allocation details
         * In order to minimize the memory management entire allocation
         * is done in one step.
         */
        m_out_mem_ptr = (OMX_BUFFERHEADERTYPE  *)calloc(nBufHdrSize,1);
 if (m_out_mem_ptr == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_out_mem_ptr");
 return OMX_ErrorInsufficientResources;
 }

        m_pOutput_pmem = (struct pmem *) calloc(sizeof (struct pmem), m_sOutPortDef.nBufferCountActual);
 if (m_pOutput_pmem == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pOutput_pmem");
 return OMX_ErrorInsufficientResources;
 }
#ifdef USE_ION
        m_pOutput_ion = (struct venc_ion *) calloc(sizeof (struct venc_ion), m_sOutPortDef.nBufferCountActual);
 if (m_pOutput_ion == NULL) {
            DEBUG_PRINT_ERROR("ERROR: calloc() Failed for m_pOutput_ion");
 return OMX_ErrorInsufficientResources;
 }
#endif
 if (m_out_mem_ptr) {
            bufHdr          =  m_out_mem_ptr;
            DEBUG_PRINT_LOW("Memory Allocation Succeeded for OUT port%p",m_out_mem_ptr);
 for (i=0; i < m_sOutPortDef.nBufferCountActual ; i++) {
                bufHdr->nSize              = sizeof(OMX_BUFFERHEADERTYPE);
                bufHdr->nVersion.nVersion  = OMX_SPEC_VERSION;
                bufHdr->nAllocLen          = bytes;
                bufHdr->nFilledLen         = 0;
                bufHdr->pAppPrivate        = appData;
                bufHdr->nOutputPortIndex   = PORT_INDEX_OUT;
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
            DEBUG_PRINT_ERROR("ERROR: Output buf mem alloc failed[0x%p]",m_out_mem_ptr);
            eRet =  OMX_ErrorInsufficientResources;
 }
 }

 for (i=0; i< m_sOutPortDef.nBufferCountActual; i++) {
 if (BITMASK_ABSENT(&m_out_bm_count,i)) {
 break;
 }
 }

 if (eRet == OMX_ErrorNone) {
 if (i < m_sOutPortDef.nBufferCountActual) {

             *bufferHdr = (m_out_mem_ptr + i );
             (*bufferHdr)->pBuffer = (OMX_U8 *)buffer;
             (*bufferHdr)->pAppPrivate = appData;
 
             if (!m_use_output_pmem) {
 #ifdef USE_ION
#ifdef _MSM8974_
                align_size = (m_sOutPortDef.nBufferSize + (SZ_4K - 1)) & ~(SZ_4K - 1);
                m_pOutput_ion[i].ion_device_fd = alloc_map_ion_memory(align_size,
 &m_pOutput_ion[i].ion_alloc_data,
 &m_pOutput_ion[i].fd_ion_data,0);
#else
                m_pOutput_ion[i].ion_device_fd = alloc_map_ion_memory(
                        m_sOutPortDef.nBufferSize,
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
                    DEBUG_PRINT_ERROR("ERROR: /dev/pmem_adsp open() Failed");
 return OMX_ErrorInsufficientResources;
 }
#endif
                m_pOutput_pmem[i].size = m_sOutPortDef.nBufferSize;
                m_pOutput_pmem[i].offset = 0;

                m_pOutput_pmem[i].buffer = (OMX_U8 *)SECURE_BUFPTR;
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
                        DEBUG_PRINT_ERROR("ERROR: mmap() Failed");
                    close(m_pOutput_pmem[i].fd);
#ifdef USE_ION
                    free_ion_memory(&m_pOutput_ion[i]);
#endif
 return OMX_ErrorInsufficientResources;
 }
 }
 } else {
                OMX_QCOM_PLATFORM_PRIVATE_PMEM_INFO *pParam = reinterpret_cast<OMX_QCOM_PLATFORM_PRIVATE_PMEM_INFO*>((*bufferHdr)->pAppPrivate);
                DEBUG_PRINT_LOW("Inside qcom_ext pParam: %p", pParam);

 if (pParam) {
                    DEBUG_PRINT_LOW("Inside qcom_ext with luma:(fd:%lu,offset:0x%x)", pParam->pmem_fd, (int)pParam->offset);
                    m_pOutput_pmem[i].fd = pParam->pmem_fd;
                    m_pOutput_pmem[i].offset = pParam->offset;
                    m_pOutput_pmem[i].size = m_sOutPortDef.nBufferSize;
                    m_pOutput_pmem[i].buffer = (unsigned char *)buffer;
 } else {
                    DEBUG_PRINT_ERROR("ERROR: Invalid AppData given for PMEM o/p UseBuffer case");
 return OMX_ErrorBadParameter;
 }
                buf_addr = (unsigned char *)buffer;
 }

            DEBUG_PRINT_LOW("use_out:: bufhdr = %p, pBuffer = %p, m_pOutput_pmem[i].buffer = %p",
 (*bufferHdr), (*bufferHdr)->pBuffer, m_pOutput_pmem[i].buffer);
 if (dev_use_buf(&m_pOutput_pmem[i],PORT_INDEX_OUT,i) != true) {

                 DEBUG_PRINT_ERROR("ERROR: dev_use_buf Failed for o/p buf");
                 return OMX_ErrorInsufficientResources;
             }

            BITMASK_SET(&m_out_bm_count,i);
         } else {
             DEBUG_PRINT_ERROR("ERROR: All o/p Buffers have been Used, invalid use_buf call for "
                     "index = %u", i);
            eRet = OMX_ErrorInsufficientResources;
 }
 }
 return eRet;
}
