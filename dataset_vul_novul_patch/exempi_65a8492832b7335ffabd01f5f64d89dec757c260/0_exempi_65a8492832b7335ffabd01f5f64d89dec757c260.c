Chunk::Chunk( ContainerChunk* parent, RIFF_MetaHandler* handler, bool skip, ChunkType c )
{
	chunkType = c; // base class assumption
	this->parent = parent;
	this->oldSize = 0;
	this->hasChange = false; // [2414649] valid assumption at creation time

	XMP_IO* file = handler->parent->ioRef;
 
 	this->oldPos = file->Offset();
 	this->id = XIO::ReadUns32_LE( file );
	this->oldSize = XIO::ReadUns32_LE( file );
	this->oldSize += 8;
 
 	XMP_Int64 chunkEnd = this->oldPos + this->oldSize;
	if ( parent != 0 ) chunkLimit = parent->oldPos + parent->oldSize;
	if ( chunkEnd > chunkLimit ) {
		bool isUpdate = XMP_OptionIsSet ( handler->parent->openFlags, kXMPFiles_OpenForUpdate );
		bool repairFile = XMP_OptionIsSet ( handler->parent->openFlags, kXMPFiles_OpenRepairFile );
		if ( (! isUpdate) || (repairFile && (parent == 0)) ) {
			this->oldSize = chunkLimit - this->oldPos;
		} else {
			XMP_Throw ( "Bad RIFF chunk size", kXMPErr_BadFileFormat );
		}
	}

	this->newSize = this->oldSize;
	this->needSizeFix = false;

	if ( skip ) file->Seek ( (this->oldSize - 8), kXMP_SeekFromCurrent );

	if ( this->parent != NULL )
	{
		this->parent->children.push_back( this );
		if( this->chunkType == chunk_VALUE )
			this->parent->childmap.insert( std::make_pair( this->id, (ValueChunk*) this ) );
	}
}
