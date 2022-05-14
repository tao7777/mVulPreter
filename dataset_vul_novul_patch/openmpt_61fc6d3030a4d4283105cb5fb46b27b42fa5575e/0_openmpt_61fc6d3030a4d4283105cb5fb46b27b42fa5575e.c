static void ConvertLoopSequence(ModSample &smp, STPLoopList &loopList)
{
	if(!smp.HasSampleData() || loopList.size() < 2) return;

	ModSample newSmp = smp;
	newSmp.nLength = 0;
	newSmp.pSample = nullptr;
 
 	size_t numLoops = loopList.size();
 
	// Get the total length of the sample after combining all looped sections
 	for(size_t i = 0; i < numLoops; i++)
 	{
 		STPLoopInfo &info = loopList[i];
 
		// If adding this loop would cause the sample length to exceed maximum,
		if(info.loopStart >= smp.nLength
			|| smp.nLength - info.loopStart < info.loopLength
			|| newSmp.nLength > MAX_SAMPLE_LENGTH - info.loopLength)
 		{
 			numLoops = i;
 			break;
		}

		newSmp.nLength += info.loopLength;
	}

	if(!newSmp.AllocateSample())
	{
		return;
	}

	SmpLength start = 0;

	for(size_t i = 0; i < numLoops; i++)
	{
		STPLoopInfo &info = loopList[i];

		memcpy(newSmp.pSample8 + start, smp.pSample8 + info.loopStart, info.loopLength);

		info.loopStart = start;
		if(i > 0 && i <= mpt::size(newSmp.cues))
		{
			newSmp.cues[i - 1] = start;
		}
		start += info.loopLength;
	}

	smp.FreeSample();
	smp = newSmp;

	smp.nLoopStart = 0;
	smp.nLoopEnd = smp.nLength;
	smp.uFlags.set(CHN_LOOP);
}
