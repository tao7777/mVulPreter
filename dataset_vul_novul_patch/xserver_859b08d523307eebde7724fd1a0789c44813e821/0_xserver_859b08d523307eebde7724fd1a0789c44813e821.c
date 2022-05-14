ProcXIChangeHierarchy(ClientPtr client)
{
    xXIAnyHierarchyChangeInfo *any;
    size_t len;			/* length of data remaining in request */
    int rc = Success;
    int flags[MAXDEVICES] = { 0 };

    REQUEST(xXIChangeHierarchyReq);
    REQUEST_AT_LEAST_SIZE(xXIChangeHierarchyReq);

     if (!stuff->num_changes)
         return rc;
 
    len = ((size_t)stuff->length << 2) - sizeof(xXIChangeHierarchyReq);
 
     any = (xXIAnyHierarchyChangeInfo *) &stuff[1];
     while (stuff->num_changes--) {
        if (len < sizeof(xXIAnyHierarchyChangeInfo)) {
            rc = BadLength;
            goto unwind;
        }

        SWAPIF(swaps(&any->type));
        SWAPIF(swaps(&any->length));

        if (len < ((size_t)any->length << 2))
            return BadLength;

#define CHANGE_SIZE_MATCH(type) \
    do { \
        if ((len < sizeof(type)) || (any->length != (sizeof(type) >> 2))) { \
            rc = BadLength; \
            goto unwind; \
        } \
    } while(0)

        switch (any->type) {
        case XIAddMaster:
        {
            xXIAddMasterInfo *c = (xXIAddMasterInfo *) any;

            /* Variable length, due to appended name string */
            if (len < sizeof(xXIAddMasterInfo)) {
                rc = BadLength;
                goto unwind;
            }
            SWAPIF(swaps(&c->name_len));
            if (c->name_len > (len - sizeof(xXIAddMasterInfo))) {
                rc = BadLength;
                goto unwind;
            }

            rc = add_master(client, c, flags);
            if (rc != Success)
                goto unwind;
        }
            break;
        case XIRemoveMaster:
        {
            xXIRemoveMasterInfo *r = (xXIRemoveMasterInfo *) any;

            CHANGE_SIZE_MATCH(xXIRemoveMasterInfo);
            rc = remove_master(client, r, flags);
            if (rc != Success)
                goto unwind;
        }
            break;
        case XIDetachSlave:
        {
            xXIDetachSlaveInfo *c = (xXIDetachSlaveInfo *) any;

            CHANGE_SIZE_MATCH(xXIDetachSlaveInfo);
            rc = detach_slave(client, c, flags);
            if (rc != Success)
                goto unwind;
        }
            break;
        case XIAttachSlave:
        {
            xXIAttachSlaveInfo *c = (xXIAttachSlaveInfo *) any;

            CHANGE_SIZE_MATCH(xXIAttachSlaveInfo);
            rc = attach_slave(client, c, flags);
            if (rc != Success)
                goto unwind;
        }
            break;
        }

        len -= any->length * 4;
        any = (xXIAnyHierarchyChangeInfo *) ((char *) any + any->length * 4);
    }

 unwind:

    XISendDeviceHierarchyEvent(flags);
    return rc;
}
