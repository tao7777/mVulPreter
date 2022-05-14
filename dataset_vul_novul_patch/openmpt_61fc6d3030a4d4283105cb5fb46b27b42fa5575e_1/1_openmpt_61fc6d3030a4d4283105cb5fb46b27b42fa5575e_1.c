bool CSoundFile::ReadSTP(FileReader &file, ModLoadingFlags loadFlags)
{
	file.Rewind();

	STPFileHeader fileHeader;
	if(!file.ReadStruct(fileHeader))
	{
		return false;
	}
	if(!ValidateHeader(fileHeader))
	{
		return false;
	}
	if(loadFlags == onlyVerifyHeader)
	{
		return true;
	}

	InitializeGlobals(MOD_TYPE_STP);

	m_nChannels = 4;
	m_nSamples = 0;

	m_nDefaultSpeed = fileHeader.speed;
	m_nDefaultTempo = ConvertTempo(fileHeader.timerCount);

	m_nMinPeriod = 14 * 4;
	m_nMaxPeriod = 3424 * 4;

 	ReadOrderFromArray(Order(), fileHeader.orderList, fileHeader.numOrders);
 
 	std::vector<STPLoopList> loopInfo;
 	std::vector<SAMPLEINDEX> nonLooped;
 
	SAMPLEINDEX samplesInFile = 0;

	for(SAMPLEINDEX smp = 0; smp < fileHeader.numSamples; smp++)
	{
		SAMPLEINDEX actualSmp = file.ReadUint16BE();
		if(actualSmp == 0 || actualSmp >= MAX_SAMPLES)
			return false;
		uint32 chunkSize = fileHeader.sampleStructSize;
		if(fileHeader.version == 2)
			chunkSize = file.ReadUint32BE() - 2;
		FileReader chunk = file.ReadChunk(chunkSize);

		samplesInFile = m_nSamples = std::max(m_nSamples, actualSmp);

		ModSample &mptSmp = Samples[actualSmp];
		mptSmp.Initialize(MOD_TYPE_MOD);

		if(fileHeader.version < 2)
		{
			chunk.ReadString<mpt::String::maybeNullTerminated>(mptSmp.filename, 31);
			chunk.Skip(1);
			chunk.ReadString<mpt::String::maybeNullTerminated>(m_szNames[actualSmp], 30);
		} else
		{
			std::string str;
			chunk.ReadNullString(str, 257);
			mpt::String::Copy(mptSmp.filename, str);
			chunk.Skip(1);
			chunk.ReadNullString(str, 31);
			mpt::String::Copy(m_szNames[actualSmp], str);
			if(chunk.GetPosition() % 2u)
				chunk.Skip(1);
		}

		STPSampleHeader sampleHeader;
		chunk.ReadStruct(sampleHeader);
		sampleHeader.ConvertToMPT(mptSmp);

		if(fileHeader.version == 2)
		{
			mptSmp.nFineTune = static_cast<int8>(sampleHeader.finetune << 3);
		}

		if(fileHeader.version >= 1)
		{
			nonLooped.resize(samplesInFile);
			loopInfo.resize(samplesInFile);
			STPLoopList &loopList = loopInfo[actualSmp - 1];
			loopList.clear();

			uint16 numLoops = file.ReadUint16BE();
			loopList.reserve(numLoops);

			STPLoopInfo loop;
			loop.looped = loop.nonLooped = 0;

			if(numLoops == 0 && mptSmp.uFlags[CHN_LOOP])
			{
				loop.loopStart  = mptSmp.nLoopStart;
				loop.loopLength = mptSmp.nLoopEnd - mptSmp.nLoopStart;
				loopList.push_back(loop);
			} else for(uint16 i = 0; i < numLoops; i++)
			{
				loop.loopStart  = file.ReadUint32BE();
				loop.loopLength = file.ReadUint32BE();
				loopList.push_back(loop);
			}
		}
	}

	uint16 numPatterns = 128;
	if(fileHeader.version == 0)
		numPatterns = file.ReadUint16BE();

	uint16 patternLength = fileHeader.patternLength;
	CHANNELINDEX channels = 4;
	if(fileHeader.version > 0)
	{
		FileReader::off_t patOffset = file.GetPosition();
		for(uint16 pat = 0; pat < numPatterns; pat++)
		{
			PATTERNINDEX actualPat = file.ReadUint16BE();
			if(actualPat == 0xFFFF)
				break;

			patternLength = file.ReadUint16BE();
			channels = file.ReadUint16BE();
			m_nChannels = std::max(m_nChannels, channels);

			file.Skip(channels * patternLength * 4u);
		}
		file.Seek(patOffset);
		if(m_nChannels > MAX_BASECHANNELS)
			return false;
	}

	struct ChannelMemory
	{
		uint8 autoFinePorta, autoPortaUp, autoPortaDown, autoVolSlide, autoVibrato;
		uint8 vibratoMem, autoTremolo, autoTonePorta, tonePortaMem;
 	};
 	std::vector<ChannelMemory> channelMemory(m_nChannels);
 	uint8 globalVolSlide = 0;
 
 	for(uint16 pat = 0; pat < numPatterns; pat++)
 	{
		PATTERNINDEX actualPat = pat;

		if(fileHeader.version > 0)
		{
			actualPat = file.ReadUint16BE();
			if(actualPat == 0xFFFF)
				break;

			patternLength = file.ReadUint16BE();
			channels = file.ReadUint16BE();
		}

		if(!(loadFlags & loadPatternData) || !Patterns.Insert(actualPat, patternLength))
		{
			file.Skip(channels * patternLength * 4u);
			continue;
		}

		for(ROWINDEX row = 0; row < patternLength; row++)
		{
			auto rowBase = Patterns[actualPat].GetRow(row);

			bool didGlobalVolSlide = false;
 
 			bool shouldDelay;
			switch(fileHeader.speedFrac & 3)
 			{
 			default: shouldDelay = false; break;
			case 1: shouldDelay = (row & 3) == 0; break;
			case 2: shouldDelay = (row & 1) == 0; break;
			case 3: shouldDelay = (row & 3) != 3; break;
			}

			for(CHANNELINDEX chn = 0; chn < channels; chn++)
			{
				ChannelMemory &chnMem = channelMemory[chn];
				ModCommand &m = rowBase[chn];
				uint8 data[4];
				file.ReadArray(data);

				m.instr   = data[0];
				m.note    = data[1];
				m.command = data[2];
				m.param   = data[3];

				if(m.note)
				{
					m.note += 24 + NOTE_MIN;
					chnMem = ChannelMemory();
				}

				uint8 swapped = (m.param >> 4) | (m.param << 4);

				if((m.command & 0xF0) == 0xF0)
				{
					uint16 ciaTempo = (static_cast<uint16>(m.command & 0x0F) << 8) | m.param;
					if(ciaTempo)
					{
						m.param = mpt::saturate_cast<ModCommand::PARAM>(Util::Round(ConvertTempo(ciaTempo).ToDouble()));
						m.command = CMD_TEMPO;
					} else
					{
						m.command = CMD_NONE;
					}
				} else switch(m.command)
				{
				case 0x00: // arpeggio
					if(m.param)
						m.command = CMD_ARPEGGIO;
					break;

				case 0x01: // portamento up
					m.command = CMD_PORTAMENTOUP;
					break;

				case 0x02: // portamento down
					m.command = CMD_PORTAMENTODOWN;
					break;

				case 0x03: // auto fine portamento up
					chnMem.autoFinePorta = 0x10 | std::min(m.param, ModCommand::PARAM(15));
					chnMem.autoPortaUp = 0;
 					chnMem.autoPortaDown = 0;
 					chnMem.autoTonePorta = 0;
 
					m.command = m.param = 0;
 					break;
 
 				case 0x04: // auto fine portamento down
					chnMem.autoFinePorta = 0x20 | std::min(m.param, ModCommand::PARAM(15));
					chnMem.autoPortaUp = 0;
 					chnMem.autoPortaDown = 0;
 					chnMem.autoTonePorta = 0;
 
					m.command = m.param = 0;
 					break;
 
 				case 0x05: // auto portamento up
					chnMem.autoFinePorta = 0;
					chnMem.autoPortaUp = m.param;
 					chnMem.autoPortaDown = 0;
 					chnMem.autoTonePorta = 0;
 
					m.command = m.param = 0;
 					break;
 
 				case 0x06: // auto portamento down
					chnMem.autoFinePorta = 0;
					chnMem.autoPortaUp = 0;
 					chnMem.autoPortaDown = m.param;
 					chnMem.autoTonePorta = 0;
 
					m.command = m.param = 0;
 					break;
 
 				case 0x07: // set global volume
					m.command = CMD_GLOBALVOLUME;
					globalVolSlide = 0;
					break;
 
 				case 0x08: // auto global fine volume slide
 					globalVolSlide = swapped;
					m.command = m.param = 0;
 					break;
 
 				case 0x09: // fine portamento up
					m.command = CMD_MODCMDEX;
					m.param = 0x10 | std::min(m.param, ModCommand::PARAM(15));
					break;

				case 0x0A: // fine portamento down
					m.command = CMD_MODCMDEX;
					m.param = 0x20 | std::min(m.param, ModCommand::PARAM(15));
					break;
 
 				case 0x0B: // auto fine volume slide
 					chnMem.autoVolSlide = swapped;
					m.command = m.param = 0;
 					break;
 
 				case 0x0C: // set volume
 					m.volcmd = VOLCMD_VOLUME;
 					m.vol = m.param;
 					chnMem.autoVolSlide = 0;
					m.command = m.param = 0;
 					break;
 
 				case 0x0D: // volume slide (param is swapped compared to .mod)
					if(m.param & 0xF0)
					{
						m.volcmd = VOLCMD_VOLSLIDEDOWN;
						m.vol = m.param >> 4;
					} else if(m.param & 0x0F)
					{
						m.volcmd = VOLCMD_VOLSLIDEUP;
 						m.vol = m.param & 0xF;
 					}
 					chnMem.autoVolSlide = 0;
					m.command = m.param = 0;
 					break;
 
 				case 0x0E: // set filter (also uses opposite value compared to .mod)
					m.command = CMD_MODCMDEX;
					m.param = 1 ^ (m.param ? 1 : 0);
					break;
 
 				case 0x0F: // set speed
 					m.command = CMD_SPEED;
					fileHeader.speedFrac = m.param & 0xF;
 					m.param >>= 4;
 					break;
 
 				case 0x10: // auto vibrato
 					chnMem.autoVibrato = m.param;
 					chnMem.vibratoMem = 0;
					m.command = m.param = 0;
 					break;
 
 				case 0x11: // auto tremolo
 					if(m.param & 0xF)
 						chnMem.autoTremolo = m.param;
 					else
 						chnMem.autoTremolo = 0;
					m.command = m.param = 0;
 					break;
 
 				case 0x12: // pattern break
					m.command = CMD_PATTERNBREAK;
					break;

				case 0x13: // auto tone portamento
					chnMem.autoFinePorta = 0;
					chnMem.autoPortaUp = 0;
					chnMem.autoPortaDown = 0;
 					chnMem.autoTonePorta = m.param;
 
 					chnMem.tonePortaMem = 0;
					m.command = m.param = 0;
 					break;
 
 				case 0x14: // position jump
					m.command = CMD_POSITIONJUMP;
					break;

				case 0x16: // start loop sequence
					if(m.instr && m.instr <= loopInfo.size())
					{
						STPLoopList &loopList = loopInfo[m.instr - 1];

						m.param--;
						if(m.param < std::min(mpt::size(ModSample().cues), loopList.size()))
						{
							m.volcmd = VOLCMD_OFFSET;
							m.vol = m.param;
 						}
 					}
 
					m.command = m.param = 0;
 					break;
 
 				case 0x17: // play only loop nn
					if(m.instr && m.instr <= loopInfo.size())
					{
						STPLoopList &loopList = loopInfo[m.instr - 1];

						m.param--;
						if(m.param < loopList.size())
						{
							if(!loopList[m.param].looped && m_nSamples < MAX_SAMPLES - 1)
								loopList[m.param].looped = ++m_nSamples;
							m.instr = static_cast<ModCommand::INSTR>(loopList[m.param].looped);
 						}
 					}
 
					m.command = m.param = 0;
 					break;
 
 				case 0x18: // play sequence without loop
					if(m.instr && m.instr <= loopInfo.size())
					{
						STPLoopList &loopList = loopInfo[m.instr - 1];

						m.param--;
						if(m.param < std::min(mpt::size(ModSample().cues), loopList.size()))
						{
							m.volcmd = VOLCMD_OFFSET;
							m.vol = m.param;
						}
						if(!nonLooped[m.instr - 1] && m_nSamples < MAX_SAMPLES - 1)
							nonLooped[m.instr - 1] = ++m_nSamples;
 						m.instr = static_cast<ModCommand::INSTR>(nonLooped[m.instr - 1]);
 					}
 
					m.command = m.param = 0;
 					break;
 
 				case 0x19: // play only loop nn without loop
					if(m.instr && m.instr <= loopInfo.size())
					{
						STPLoopList &loopList = loopInfo[m.instr - 1];

						m.param--;
						if(m.param < loopList.size())
						{
							if(!loopList[m.param].nonLooped && m_nSamples < MAX_SAMPLES-1)
								loopList[m.param].nonLooped = ++m_nSamples;
							m.instr = static_cast<ModCommand::INSTR>(loopList[m.param].nonLooped);
 						}
 					}
 
					m.command = m.param = 0;
 					break;
 
 				case 0x1D: // fine volume slide (nibble order also swapped)
					m.command = CMD_VOLUMESLIDE;
					m.param = swapped;
					if(m.param & 0xF0) // slide down
						m.param |= 0x0F;
					else if(m.param & 0x0F)
						m.param |= 0xF0;
					break;

				case 0x20: // "delayed fade"
 					if(m.param & 0xF0)
 					{
 						chnMem.autoVolSlide = m.param >> 4;
						m.command = m.param = 0;
 					} else
 					{
 						m.command = CMD_MODCMDEX;
						m.param = 0xC0 | (m.param & 0xF);
					}
					break;

				case 0x21: // note delay
					m.command = CMD_MODCMDEX;
					m.param = 0xD0 | std::min(m.param, ModCommand::PARAM(15));
					break;

				case 0x22: // retrigger note
					m.command = CMD_MODCMDEX;
					m.param = 0x90 | std::min(m.param, ModCommand::PARAM(15));
					break;

				case 0x49: // set sample offset
					m.command = CMD_OFFSET;
					break;

				case 0x4E: // other protracker commands (pattern loop / delay)
 					if((m.param & 0xF0) == 0x60 || (m.param & 0xF0) == 0xE0)
 						m.command = CMD_MODCMDEX;
 					else
						m.command = m.param = 0;
 					break;
 
 				case 0x4F: // set speed/tempo
 					if(m.param < 0x20)
 					{
 						m.command = CMD_SPEED;
						fileHeader.speedFrac = 0;
 					} else
 					{
 						m.command = CMD_TEMPO;
					}
					break;

				default:
					m.command = CMD_NONE;
					break;
				}

 				bool didVolSlide = false;
 
				if(chnMem.autoVolSlide && !m.volcmd)
 				{
 					if(chnMem.autoVolSlide & 0xF0)
 					{
						m.volcmd = VOLCMD_FINEVOLUP;
						m.vol = chnMem.autoVolSlide >> 4;
					} else
					{
						m.volcmd = VOLCMD_FINEVOLDOWN;
						m.vol = chnMem.autoVolSlide & 0xF;
					}
					didVolSlide = true;
				}

				if(m.command == CMD_NONE)
				{
					if(chnMem.autoPortaUp)
					{
						m.command = CMD_PORTAMENTOUP;
						m.param = chnMem.autoPortaUp;

					} else if(chnMem.autoPortaDown)
					{
						m.command = CMD_PORTAMENTODOWN;
						m.param = chnMem.autoPortaDown;
					} else if(chnMem.autoFinePorta)
					{
						m.command = CMD_MODCMDEX;
						m.param = chnMem.autoFinePorta;

					} else if(chnMem.autoTonePorta)
					{
						m.command = CMD_TONEPORTAMENTO;
						m.param = chnMem.tonePortaMem = chnMem.autoTonePorta;

					} else if(chnMem.autoVibrato)
					{
						m.command = CMD_VIBRATO;
						m.param = chnMem.vibratoMem = chnMem.autoVibrato;

					} else if(!didVolSlide && chnMem.autoVolSlide)
					{
						m.command = CMD_VOLUMESLIDE;
						m.param = chnMem.autoVolSlide;
						if(m.param & 0x0F)
							m.param |= 0xF0;
						else if(m.param & 0xF0)
							m.param |= 0x0F;
						didVolSlide = true;

					} else if(chnMem.autoTremolo)
					{
						m.command = CMD_TREMOLO;
						m.param = chnMem.autoTremolo;

					} else if(shouldDelay)
					{
						m.command = CMD_S3MCMDEX;
						m.param = 0x61;
						shouldDelay = false;

					} else if(!didGlobalVolSlide && globalVolSlide)
					{
						m.command = CMD_GLOBALVOLSLIDE;
						m.param = globalVolSlide;
						if(m.param & 0x0F)
							m.param |= 0xF0;
						else if(m.param & 0xF0)
							m.param |= 0x0F;

						didGlobalVolSlide = true;
					}
				}
			}

		}
	}

	m_nSamplePreAmp = 256 / m_nChannels;
	SetupMODPanning(true);

	if(fileHeader.version > 0)
	{
		while(file.CanRead(2))
		{
			uint16 scriptNum = file.ReadUint16BE();
			if(scriptNum == 0xFFFF)
				break;

			file.Skip(2);
			uint32 length = file.ReadUint32BE();
			file.Skip(length);
		}

		file.Skip(17 * 2);
	}

	if(loadFlags & loadSampleData)
	{
		for(SAMPLEINDEX smp = 1; smp <= samplesInFile; smp++) if(Samples[smp].nLength)
		{
			SampleIO(
				SampleIO::_8bit,
				SampleIO::mono,
				SampleIO::littleEndian,
				SampleIO::signedPCM)
				.ReadSample(Samples[smp], file);

			if(smp > loopInfo.size())
				continue;

			ConvertLoopSequence(Samples[smp], loopInfo[smp - 1]);

			if(nonLooped[smp - 1])
			{
				ConvertLoopSlice(Samples[smp], Samples[nonLooped[smp - 1]], 0, Samples[smp].nLength, false);
			}

			for(const auto &info : loopInfo[smp - 1])
			{
				if(info.looped)
				{
					ConvertLoopSlice(Samples[smp], Samples[info.looped], info.loopStart, info.loopLength, true);
				}
				if(info.nonLooped)
				{
					ConvertLoopSlice(Samples[smp], Samples[info.nonLooped], info.loopStart, info.loopLength, false);
				}
			}
		}
	}

	return true;
}
