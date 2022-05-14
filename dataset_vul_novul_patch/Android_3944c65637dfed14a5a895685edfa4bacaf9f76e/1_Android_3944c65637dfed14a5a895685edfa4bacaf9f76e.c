void unmarshallAudioAttributes(const Parcel& parcel, audio_attributes_t *attributes)
{
    attributes->usage = (audio_usage_t) parcel.readInt32();
    attributes->content_type = (audio_content_type_t) parcel.readInt32();
    attributes->source = (audio_source_t) parcel.readInt32();
    attributes->flags = (audio_flags_mask_t) parcel.readInt32();
 const bool hasFlattenedTag = (parcel.readInt32() == kAudioAttributesMarshallTagFlattenTags);
 if (hasFlattenedTag) {
 String16 tags = parcel.readString16();
 ssize_t realTagSize = utf16_to_utf8_length(tags.string(), tags.size());
 if (realTagSize <= 0) {
            strcpy(attributes->tags, "");
 } else {

             size_t tagSize = realTagSize > AUDIO_ATTRIBUTES_TAGS_MAX_SIZE - 1 ?
                     AUDIO_ATTRIBUTES_TAGS_MAX_SIZE - 1 : realTagSize;
            utf16_to_utf8(tags.string(), tagSize, attributes->tags);
         }
     } else {
         ALOGE("unmarshallAudioAttributes() received unflattened tags, ignoring tag values");
        strcpy(attributes->tags, "");
 }
}
