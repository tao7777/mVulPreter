double GetGPMFSampleRate(size_t handle, uint32_t fourcc, uint32_t flags)
double GetGPMFSampleRate(size_t handle, uint32_t fourcc, uint32_t flags, double *firstsampletime, double *lastsampletime)
 {
 	mp4object *mp4 = (mp4object *)handle;
 	if (mp4 == NULL) return 0.0;

	GPMF_stream metadata_stream, *ms = &metadata_stream;
	uint32_t teststart = 0;
 	uint32_t testend = mp4->indexcount;
 	double rate = 0.0;
 
	uint32_t *payload;
	uint32_t payloadsize;
	int32_t ret;

 	if (mp4->indexcount < 1)
 		return 0.0;
 
	payload = GetPayload(handle, NULL, teststart); 
	payloadsize = GetPayloadSize(handle, teststart);
	ret = GPMF_Init(ms, payload, payloadsize);
 
 	if (ret != GPMF_OK)
 		goto cleanup;
 
 	{
		uint64_t minimumtimestamp = 0;
		uint64_t starttimestamp = 0;
		uint64_t endtimestamp = 0;
 		uint32_t startsamples = 0;
 		uint32_t endsamples = 0;
		double intercept = 0.0;


 
		while (teststart < mp4->indexcount && ret == GPMF_OK && GPMF_OK != GPMF_FindNext(ms, fourcc, GPMF_RECURSE_LEVELS))
 		{
 			teststart++;
 			payload = GetPayload(handle, payload, teststart); // second last payload
 			payloadsize = GetPayloadSize(handle, teststart);
 			ret = GPMF_Init(ms, payload, payloadsize);
 		}
 
		if (ret == GPMF_OK && payload)
 		{
			uint32_t samples = GPMF_PayloadSampleCount(ms);
 			GPMF_stream find_stream;
 			GPMF_CopyState(ms, &find_stream);
			if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TOTAL_SAMPLES, GPMF_CURRENT_LEVEL))
				startsamples = BYTESWAP32(*(uint32_t *)GPMF_RawData(&find_stream)) - samples;
 
			GPMF_CopyState(ms, &find_stream);
			if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TIME_STAMP, GPMF_CURRENT_LEVEL))
				starttimestamp = BYTESWAP64(*(uint64_t *)GPMF_RawData(&find_stream));

			if (starttimestamp) // is this earliest in the payload, examine the other streams in this early payload.
 			{
				GPMF_stream any_stream;
				GPMF_Init(&any_stream, payload, payloadsize);

				minimumtimestamp = starttimestamp;
				while (GPMF_OK == GPMF_FindNext(&any_stream, GPMF_KEY_TIME_STAMP, GPMF_RECURSE_LEVELS))
				{
					uint64_t timestamp = BYTESWAP64(*(uint64_t *)GPMF_RawData(&any_stream));
					if (timestamp < minimumtimestamp)
						minimumtimestamp = timestamp;
				}
			}
 
			testend = mp4->indexcount;
			do
			{
				testend--;// last payload with the fourcc needed
				payload = GetPayload(handle, payload, testend);
 				payloadsize = GetPayloadSize(handle, testend);
 				ret = GPMF_Init(ms, payload, payloadsize);
			} while (testend > 0 && GPMF_OK != GPMF_FindNext(ms, fourcc, GPMF_RECURSE_LEVELS));
 
			GPMF_CopyState(ms, &find_stream);
			if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TOTAL_SAMPLES, GPMF_CURRENT_LEVEL))
				endsamples = BYTESWAP32(*(uint32_t *)GPMF_RawData(&find_stream));
			else // If there is no TSMP we have to count the samples.
			{
				uint32_t i;
				for (i = teststart; i <= testend; i++)
 				{
					payload = GetPayload(handle,payload, i); // second last payload
					payloadsize = GetPayloadSize(handle, i);
					if (GPMF_OK == GPMF_Init(ms, payload, payloadsize))
						if (GPMF_OK == GPMF_FindNext(ms, fourcc, GPMF_RECURSE_LEVELS))
							endsamples += GPMF_PayloadSampleCount(ms);
				}
			}

			if (starttimestamp != 0)
			{
				uint32_t last_samples = GPMF_PayloadSampleCount(ms);
				uint32_t totaltimestamped_samples = endsamples - last_samples - startsamples;
				double time_stamp_scale = 1000000000.0; // scan for nanoseconds, microseconds to seconds, all base 10.

				GPMF_CopyState(ms, &find_stream);
				if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TIME_STAMP, GPMF_CURRENT_LEVEL))
					endtimestamp = BYTESWAP64(*(uint64_t *)GPMF_RawData(&find_stream));

				if (endtimestamp)
				{
					double approxrate = 0.0;
					if (endsamples > startsamples)
						approxrate = (double)(endsamples - startsamples) / (mp4->metadatalength * ((double)(testend - teststart + 1)) / (double)mp4->indexcount);

					if (approxrate == 0.0)
						approxrate = (double)(samples) / (mp4->metadatalength * ((double)(testend - teststart + 1)) / (double)mp4->indexcount);


					while (time_stamp_scale >= 1)
 					{
						rate = (double)(totaltimestamped_samples) / ((double)(endtimestamp - starttimestamp) / time_stamp_scale);
						if (rate*0.9 < approxrate && approxrate < rate*1.1)
							break;

						time_stamp_scale *= 0.1;
 					}
					if (time_stamp_scale < 1.0) rate = 0.0;
					intercept = (((double)minimumtimestamp - (double)starttimestamp) / time_stamp_scale) * rate;
 				}
 			}

			if (rate == 0.0) //Timestamps didn't help weren't available
 			{
				if (!(flags & GPMF_SAMPLE_RATE_PRECISE))
				{
					if (endsamples > startsamples)
						rate = (double)(endsamples - startsamples) / (mp4->metadatalength * ((double)(testend - teststart + 1)) / (double)mp4->indexcount);
 
					if (rate == 0.0)
						rate = (double)(samples) / (mp4->metadatalength * ((double)(testend - teststart + 1)) / (double)mp4->indexcount);
 
					double in, out;
					if (GPMF_OK == GetPayloadTime(handle, teststart, &in, &out))
						intercept = (double)-in * rate;
				}
				else // for increased precision, for older GPMF streams sometimes missing the total sample count 
 				{
					uint32_t payloadpos = 0, payloadcount = 0;
					double slope, top = 0.0, bot = 0.0, meanX = 0, meanY = 0;
					uint32_t *repeatarray = malloc(mp4->indexcount * 4 + 4);
					memset(repeatarray, 0, mp4->indexcount * 4 + 4);
 
					samples = 0;
 
					for (payloadpos = teststart; payloadpos <= testend; payloadpos++)
 					{
						payload = GetPayload(handle, payload, payloadpos); // second last payload
						payloadsize = GetPayloadSize(handle, payloadpos);
						ret = GPMF_Init(ms, payload, payloadsize);
 
						if (ret != GPMF_OK)
							goto cleanup;

						if (GPMF_OK == GPMF_FindNext(ms, fourcc, GPMF_RECURSE_LEVELS))
 						{
							GPMF_stream find_stream2;
							GPMF_CopyState(ms, &find_stream2);

							payloadcount++;
 
							if (GPMF_OK == GPMF_FindNext(&find_stream2, fourcc, GPMF_CURRENT_LEVEL)) // Count the instances, not the repeats
							{
								if (repeatarray)
 								{
									double in, out;
 
									do
									{
										samples++;
									} while (GPMF_OK == GPMF_FindNext(ms, fourcc, GPMF_CURRENT_LEVEL));
 
									repeatarray[payloadpos] = samples;
									meanY += (double)samples;
 
									if (GPMF_OK == GetPayloadTime(handle, payloadpos, &in, &out))
										meanX += out;
								}
							}
							else
 							{
								uint32_t repeat = GPMF_PayloadSampleCount(ms);
								samples += repeat;

								if (repeatarray)
								{
									double in, out;
 
									repeatarray[payloadpos] = samples;
									meanY += (double)samples;
 
									if (GPMF_OK == GetPayloadTime(handle, payloadpos, &in, &out))
										meanX += out;
								}
 							}
 						}
						else
						{
							repeatarray[payloadpos] = 0;
						}
 					}
 
					// Compute the line of best fit for a jitter removed sample rate.  
					// This does assume an unchanging clock, even though the IMU data can thermally impacted causing small clock changes.  
					// TODO: Next enhancement would be a low order polynominal fit the compensate for any thermal clock drift.
					if (repeatarray)
 					{
						meanY /= (double)payloadcount;
						meanX /= (double)payloadcount;
 
						for (payloadpos = teststart; payloadpos <= testend; payloadpos++)
						{
							double in, out;
							if (repeatarray[payloadpos] && GPMF_OK == GetPayloadTime(handle, payloadpos, &in, &out))
							{
								top += ((double)out - meanX)*((double)repeatarray[payloadpos] - meanY);
								bot += ((double)out - meanX)*((double)out - meanX);
							}
						}
 
						slope = top / bot;
						rate = slope;
 
						// This sample code might be useful for compare data latency between channels.
						intercept = meanY - slope * meanX;
 #if 0
						printf("%c%c%c%c start offset = %f (%.3fms) rate = %f\n", PRINTF_4CC(fourcc), intercept, 1000.0 * intercept / slope, rate);
						printf("%c%c%c%c first sample at time %.3fms\n", PRINTF_4CC(fourcc), -1000.0 * intercept / slope);
#endif
					}
					else
 					{
						rate = (double)(samples) / (mp4->metadatalength * ((double)(testend - teststart + 1)) / (double)mp4->indexcount);
 					}
 
					free(repeatarray);
				}
 			}
 
