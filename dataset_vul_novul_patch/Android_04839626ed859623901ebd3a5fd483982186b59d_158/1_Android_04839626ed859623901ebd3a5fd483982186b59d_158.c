long Segment::LoadCluster(
    long long& pos,
    long& len)
{
    for (;;)
    {
        const long result = DoLoadCluster(pos, len);
 
        if (result <= 1)
            return result;
     }
}
