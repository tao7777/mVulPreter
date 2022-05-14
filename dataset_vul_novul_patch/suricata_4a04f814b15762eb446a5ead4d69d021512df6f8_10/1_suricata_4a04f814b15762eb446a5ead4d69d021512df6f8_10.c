static int DefragTrackerReuseTest(void)
{
    int ret = 0;
    int id = 1;
    Packet *p1 = NULL;
    DefragTracker *tracker1 = NULL, *tracker2 = NULL;

    DefragInit();
 
     /* Build a packet, its not a fragment but shouldn't matter for
      * this test. */
    p1 = BuildTestPacket(id, 0, 0, 'A', 8);
     if (p1 == NULL) {
         goto end;
     }

    /* Get a tracker. It shouldn't look like its already in use. */
    tracker1 = DefragGetTracker(NULL, NULL, p1);
    if (tracker1 == NULL) {
        goto end;
    }
    if (tracker1->seen_last) {
        goto end;
    }
    if (tracker1->remove) {
        goto end;
    }
    DefragTrackerRelease(tracker1);

    /* Get a tracker again, it should be the same one. */
    tracker2 = DefragGetTracker(NULL, NULL, p1);
    if (tracker2 == NULL) {
        goto end;
    }
    if (tracker2 != tracker1) {
        goto end;
    }
    DefragTrackerRelease(tracker1);

    /* Now mark the tracker for removal. It should not be returned
     * when we get a tracker for a packet that may have the same
     * attributes. */
    tracker1->remove = 1;

    tracker2 = DefragGetTracker(NULL, NULL, p1);
    if (tracker2 == NULL) {
        goto end;
    }
    if (tracker2 == tracker1) {
        goto end;
    }
    if (tracker2->remove) {
        goto end;
    }

    ret = 1;
end:
    if (p1 != NULL) {
        SCFree(p1);
    }
    DefragDestroy();
    return ret;
}
