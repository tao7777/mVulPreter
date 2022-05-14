virDomainGetTime(virDomainPtr dom,
                 long long *seconds,
                 unsigned int *nseconds,
                 unsigned int flags)
{
    VIR_DOMAIN_DEBUG(dom, "seconds=%p, nseconds=%p, flags=%x",
                     seconds, nseconds, flags);

     virResetLastError();
 
     virCheckDomainReturn(dom, -1);
    virCheckReadOnlyGoto(dom->conn->flags, error);
 
     if (dom->conn->driver->domainGetTime) {
         int ret = dom->conn->driver->domainGetTime(dom, seconds,
                                                   nseconds, flags);
        if (ret < 0)
            goto error;
        return ret;
    }

    virReportUnsupportedError();

 error:
    virDispatchError(dom->conn);
    return -1;
}
