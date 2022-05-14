int main(int argc, char *argv[])
{
	int32_t ret = GPMF_OK;
	GPMF_stream metadata_stream, *ms = &metadata_stream;
	double metadatalength;
	uint32_t *payload = NULL; //buffer to store GPMF samples from the MP4.


	if (argc != 2)
	{
		printf("usage: %s <file_with_GPMF>\n", argv[0]);
		return -1;
 	}
 
 	size_t mp4 = OpenMP4Source(argv[1], MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE);
	if (mp4 == 0)
	{
		printf("error: %s is an invalid MP4/MOV\n", argv[1]);
		return -1;
	}

 
 	metadatalength = GetDuration(mp4);

	if (metadatalength > 0.0)
	{
		uint32_t index, payloads = GetNumberPayloads(mp4);

#if 1
		if (payloads == 1) // Printf the contents of the single payload
		{
			uint32_t payloadsize = GetPayloadSize(mp4,0);
			payload = GetPayload(mp4, payload, 0);
			if(payload == NULL)
				goto cleanup;

			ret = GPMF_Init(ms, payload, payloadsize);
			if (ret != GPMF_OK)
				goto cleanup;

			ret = GPMF_Validate(ms, GPMF_RECURSE_LEVELS); // optional
			if (GPMF_OK != ret)
			{
				printf("Invalid Structure\n");
				goto cleanup;
			}

			GPMF_ResetState(ms);
			do
			{
				PrintGPMF(ms);  // printf current GPMF KLV
			} while (GPMF_OK == GPMF_Next(ms, GPMF_RECURSE_LEVELS));
			GPMF_ResetState(ms);
			printf("\n");

		}
#endif


 		for (index = 0; index < payloads; index++)
 		{
 			uint32_t payloadsize = GetPayloadSize(mp4, index);
			double in = 0.0, out = 0.0; //times
 			payload = GetPayload(mp4, payload, index);
 			if (payload == NULL)
 				goto cleanup;

			ret = GetPayloadTime(mp4, index, &in, &out);
			if (ret != GPMF_OK)
				goto cleanup;

			ret = GPMF_Init(ms, payload, payloadsize);
			if (ret != GPMF_OK)
				goto cleanup;

#if 1		// Find all the available Streams and the data carrying FourCC
			if (index == 0) // show first payload 
			{
				ret = GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS);
				while (GPMF_OK == ret)
				{
					ret = GPMF_SeekToSamples(ms);
					if (GPMF_OK == ret) //find the last FOURCC within the stream
					{
						uint32_t key = GPMF_Key(ms);
						GPMF_SampleType type = GPMF_Type(ms);
						uint32_t elements = GPMF_ElementsInStruct(ms);
						uint32_t samples = GPMF_PayloadSampleCount(ms);

						if (samples)
						{
							printf("  STRM of %c%c%c%c ", PRINTF_4CC(key));

							if (type == GPMF_TYPE_COMPLEX)
							{
								GPMF_stream find_stream;
								GPMF_CopyState(ms, &find_stream);

								if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL))
								{
									char tmp[64];
									char *data = (char *)GPMF_RawData(&find_stream);
									int size = GPMF_RawDataSize(&find_stream);

									if (size < sizeof(tmp))
									{
										memcpy(tmp, data, size);
										tmp[size] = 0;
										printf("of type %s ", tmp);
									}
								}

							}
							else
							{
								printf("of type %c ", type);
							}

							printf("with %d sample%s ", samples, samples > 1 ? "s" : "");

							if (elements > 1)
								printf("-- %d elements per sample", elements);

							printf("\n");
						}

						ret = GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS);
					}
					else
					{
						if (ret == GPMF_ERROR_BAD_STRUCTURE) // some payload element was corrupt, skip to the next valid GPMF KLV at the previous level.
						{
							ret = GPMF_Next(ms, GPMF_CURRENT_LEVEL); // this will be the next stream if any more are present.
						}
					}
				}
				GPMF_ResetState(ms);
				printf("\n");
			}
#endif 




#if 1		// Find GPS values and return scaled doubles. 
			if (index == 0) // show first payload 
			{
				if (GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPS5"), GPMF_RECURSE_LEVELS) || //GoPro Hero5/6/7 GPS
					GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPRI"), GPMF_RECURSE_LEVELS))   //GoPro Karma GPS
				{
					uint32_t key = GPMF_Key(ms);
					uint32_t samples = GPMF_Repeat(ms);
					uint32_t elements = GPMF_ElementsInStruct(ms);
					uint32_t buffersize = samples * elements * sizeof(double);
					GPMF_stream find_stream;
					double *ptr, *tmpbuffer = malloc(buffersize);
					char units[10][6] = { "" };
					uint32_t unit_samples = 1;

					printf("MP4 Payload time %.3f to %.3f seconds\n", in, out);

					if (tmpbuffer && samples)
					{
						uint32_t i, j;

						GPMF_CopyState(ms, &find_stream);
						if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL) ||
							GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL))
						{
							char *data = (char *)GPMF_RawData(&find_stream);
							int ssize = GPMF_StructSize(&find_stream);
							unit_samples = GPMF_Repeat(&find_stream);

							for (i = 0; i < unit_samples; i++)
							{
								memcpy(units[i], data, ssize);
								units[i][ssize] = 0;
								data += ssize;
							}
						}

						GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE);  //Output scaled data as floats

						ptr = tmpbuffer;
						for (i = 0; i < samples; i++)
						{
							printf("%c%c%c%c ", PRINTF_4CC(key));
							for (j = 0; j < elements; j++)
								printf("%.3f%s, ", *ptr++, units[j%unit_samples]);

							printf("\n");
						}
						free(tmpbuffer);
					}
				}
				GPMF_ResetState(ms);
				printf("\n");
			}
#endif 
		}

#if 1
		while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS))
 		{
 			if (GPMF_OK == GPMF_SeekToSamples(ms)) //find the last FOURCC within the stream
 			{
				double in = 0.0, out = 0.0;
 				uint32_t fourcc = GPMF_Key(ms);
				double rate = GetGPMFSampleRate(mp4, fourcc, GPMF_SAMPLE_RATE_PRECISE, &in, &out);// GPMF_SAMPLE_RATE_FAST);
				printf("%c%c%c%c sampling rate = %f Hz (from %f to %f)\n", PRINTF_4CC(fourcc), rate, in, out);
 			}
 		}
 #endif


	cleanup:
		if (payload) FreePayload(payload); payload = NULL;
		CloseSource(mp4);
	}

	return ret;
}
