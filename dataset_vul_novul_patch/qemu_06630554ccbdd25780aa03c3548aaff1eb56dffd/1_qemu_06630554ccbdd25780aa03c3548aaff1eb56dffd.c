static void mptsas_fetch_request(MPTSASState *s)
{
    PCIDevice *pci = (PCIDevice *) s;
    char req[MPTSAS_MAX_REQUEST_SIZE];
    MPIRequestHeader *hdr = (MPIRequestHeader *)req;
     hwaddr addr;
     int size;
 
    if (s->state != MPI_IOC_STATE_OPERATIONAL) {
        mptsas_set_fault(s, MPI_IOCSTATUS_INVALID_STATE);
        return;
    }
     /* Read the message header from the guest first. */
     addr = s->host_mfa_high_addr | MPTSAS_FIFO_GET(s, request_post);
     pci_dma_read(pci, addr, req, sizeof(hdr));
    }
