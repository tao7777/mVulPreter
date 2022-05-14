DefragRegisterTests(void)
{
#ifdef UNITTESTS
    UtRegisterTest("DefragInOrderSimpleTest", DefragInOrderSimpleTest);
    UtRegisterTest("DefragReverseSimpleTest", DefragReverseSimpleTest);
    UtRegisterTest("DefragSturgesNovakBsdTest", DefragSturgesNovakBsdTest);
    UtRegisterTest("DefragSturgesNovakLinuxTest", DefragSturgesNovakLinuxTest);
    UtRegisterTest("DefragSturgesNovakWindowsTest",
                   DefragSturgesNovakWindowsTest);
    UtRegisterTest("DefragSturgesNovakSolarisTest",
                   DefragSturgesNovakSolarisTest);
    UtRegisterTest("DefragSturgesNovakFirstTest", DefragSturgesNovakFirstTest);
    UtRegisterTest("DefragSturgesNovakLastTest", DefragSturgesNovakLastTest);

    UtRegisterTest("DefragIPv4NoDataTest", DefragIPv4NoDataTest);
    UtRegisterTest("DefragIPv4TooLargeTest", DefragIPv4TooLargeTest);

    UtRegisterTest("IPV6DefragInOrderSimpleTest", IPV6DefragInOrderSimpleTest);
    UtRegisterTest("IPV6DefragReverseSimpleTest", IPV6DefragReverseSimpleTest);
    UtRegisterTest("IPV6DefragSturgesNovakBsdTest",
                   IPV6DefragSturgesNovakBsdTest);
    UtRegisterTest("IPV6DefragSturgesNovakLinuxTest",
                   IPV6DefragSturgesNovakLinuxTest);
    UtRegisterTest("IPV6DefragSturgesNovakWindowsTest",
                   IPV6DefragSturgesNovakWindowsTest);
    UtRegisterTest("IPV6DefragSturgesNovakSolarisTest",
                   IPV6DefragSturgesNovakSolarisTest);
    UtRegisterTest("IPV6DefragSturgesNovakFirstTest",
                   IPV6DefragSturgesNovakFirstTest);
    UtRegisterTest("IPV6DefragSturgesNovakLastTest",
                   IPV6DefragSturgesNovakLastTest);

    UtRegisterTest("DefragVlanTest", DefragVlanTest);
    UtRegisterTest("DefragVlanQinQTest", DefragVlanQinQTest);
    UtRegisterTest("DefragTrackerReuseTest", DefragTrackerReuseTest);
     UtRegisterTest("DefragTimeoutTest", DefragTimeoutTest);
     UtRegisterTest("DefragMfIpv4Test", DefragMfIpv4Test);
     UtRegisterTest("DefragMfIpv6Test", DefragMfIpv6Test);
 #endif /* UNITTESTS */
 }
