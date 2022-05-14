String TextCodecUTF8::Decode(const char* bytes,
                             wtf_size_t length,
                             FlushBehavior flush,
                             bool stop_on_error,
                             bool& saw_error) {
  const bool do_flush = flush != FlushBehavior::kDoNotFlush;
  StringBuffer<LChar> buffer(
      base::CheckAdd(partial_sequence_size_, length).ValueOrDie());
 
   const uint8_t* source = reinterpret_cast<const uint8_t*>(bytes);
   const uint8_t* end = source + length;
  const uint8_t* aligned_end = AlignToMachineWord(end);
  LChar* destination = buffer.Characters();

  do {
    if (partial_sequence_size_) {
      LChar* destination_for_handle_partial_sequence = destination;
      const uint8_t* source_for_handle_partial_sequence = source;
      if (HandlePartialSequence(destination_for_handle_partial_sequence,
                                source_for_handle_partial_sequence, end,
                                do_flush, stop_on_error, saw_error)) {
        source = source_for_handle_partial_sequence;
        goto upConvertTo16Bit;
      }
      destination = destination_for_handle_partial_sequence;
      source = source_for_handle_partial_sequence;
      if (partial_sequence_size_)
        break;
    }

    while (source < end) {
      if (IsASCII(*source)) {
        if (IsAlignedToMachineWord(source)) {
          while (source < aligned_end) {
            MachineWord chunk =
                *reinterpret_cast_ptr<const MachineWord*>(source);
            if (!IsAllASCII<LChar>(chunk))
              break;
            CopyASCIIMachineWord(destination, source);
            source += sizeof(MachineWord);
            destination += sizeof(MachineWord);
          }
          if (source == end)
            break;
          if (!IsASCII(*source))
            continue;
        }
        *destination++ = *source++;
        continue;
      }
      int count = NonASCIISequenceLength(*source);
      int character;
      if (count == 0) {
        character = kNonCharacter1;
      } else {
        if (count > end - source) {
          SECURITY_DCHECK(end - source <
                          static_cast<ptrdiff_t>(sizeof(partial_sequence_)));
          DCHECK(!partial_sequence_size_);
          partial_sequence_size_ = static_cast<wtf_size_t>(end - source);
          memcpy(partial_sequence_, source, partial_sequence_size_);
          source = end;
          break;
        }
        character = DecodeNonASCIISequence(source, count);
      }
      if (IsNonCharacter(character)) {
        saw_error = true;
        if (stop_on_error)
          break;

        goto upConvertTo16Bit;
      }
      if (character > 0xff)
        goto upConvertTo16Bit;

      source += count;
      *destination++ = static_cast<LChar>(character);
    }
  } while (do_flush && partial_sequence_size_);

  buffer.Shrink(static_cast<wtf_size_t>(destination - buffer.Characters()));

   return String::Adopt(buffer);
 
 upConvertTo16Bit:
  StringBuffer<UChar> buffer16(
      base::CheckAdd(partial_sequence_size_, length).ValueOrDie());
 
   UChar* destination16 = buffer16.Characters();
 
  for (LChar* converted8 = buffer.Characters(); converted8 < destination;)
    *destination16++ = *converted8++;

  do {
    if (partial_sequence_size_) {
      UChar* destination_for_handle_partial_sequence = destination16;
      const uint8_t* source_for_handle_partial_sequence = source;
      HandlePartialSequence(destination_for_handle_partial_sequence,
                            source_for_handle_partial_sequence, end, do_flush,
                            stop_on_error, saw_error);
      destination16 = destination_for_handle_partial_sequence;
      source = source_for_handle_partial_sequence;
      if (partial_sequence_size_)
        break;
    }

    while (source < end) {
      if (IsASCII(*source)) {
        if (IsAlignedToMachineWord(source)) {
          while (source < aligned_end) {
            MachineWord chunk =
                *reinterpret_cast_ptr<const MachineWord*>(source);
            if (!IsAllASCII<LChar>(chunk))
              break;
            CopyASCIIMachineWord(destination16, source);
            source += sizeof(MachineWord);
            destination16 += sizeof(MachineWord);
          }
          if (source == end)
            break;
          if (!IsASCII(*source))
            continue;
        }
        *destination16++ = *source++;
        continue;
      }
      int count = NonASCIISequenceLength(*source);
      int character;
      if (count == 0) {
        character = kNonCharacter1;
      } else {
        if (count > end - source) {
          SECURITY_DCHECK(end - source <
                          static_cast<ptrdiff_t>(sizeof(partial_sequence_)));
          DCHECK(!partial_sequence_size_);
          partial_sequence_size_ = static_cast<wtf_size_t>(end - source);
          memcpy(partial_sequence_, source, partial_sequence_size_);
          source = end;
          break;
        }
        character = DecodeNonASCIISequence(source, count);
      }
      if (IsNonCharacter(character)) {
        saw_error = true;
        if (stop_on_error)
          break;
        *destination16++ = kReplacementCharacter;
        source -= character;
        continue;
      }
      source += count;
      destination16 = AppendCharacter(destination16, character);
    }
  } while (do_flush && partial_sequence_size_);

  buffer16.Shrink(
      static_cast<wtf_size_t>(destination16 - buffer16.Characters()));

  return String::Adopt(buffer16);
}
