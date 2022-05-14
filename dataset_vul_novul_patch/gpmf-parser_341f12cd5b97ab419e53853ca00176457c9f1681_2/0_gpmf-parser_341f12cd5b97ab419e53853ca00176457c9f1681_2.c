double GetGPMFSampleRateAndTimes(size_t handle, GPMF_stream *gs, double rate, uint32_t index, double *in, double *out)
					GPMF_CopyState(ms, &find_stream);
					if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TIME_OFFSET, GPMF_CURRENT_LEVEL))
						GPMF_FormattedData(&find_stream, &timo, 4, 0, 1);
 
					double first, last;
					first = -intercept / rate - timo;
					last = first + (double)totalsamples / rate;
 
					//printf("%c%c%c%c first sample at time %.3fms, last at %.3fms\n", PRINTF_4CC(fourcc), 1000.0*first, 1000.0*last);
 
					if (firstsampletime) *firstsampletime = first;
 
					if (lastsampletime) *lastsampletime = last;
				}
			}
		}
 	}
 
cleanup:
	if (payload)
		FreePayload(payload);
	payload = NULL;
 
 	return rate;
 }
