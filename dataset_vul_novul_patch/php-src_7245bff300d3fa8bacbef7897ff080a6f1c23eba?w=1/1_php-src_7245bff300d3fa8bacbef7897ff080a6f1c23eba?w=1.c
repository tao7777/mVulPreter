PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(SplFileInfo, spl_filesystem_object_new, spl_SplFileInfo_functions);
	memcpy(&spl_filesystem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_filesystem_object_handlers.clone_obj       = spl_filesystem_object_clone;
	spl_filesystem_object_handlers.cast_object     = spl_filesystem_object_cast;
	spl_filesystem_object_handlers.get_debug_info  = spl_filesystem_object_get_debug_info;
	spl_ce_SplFileInfo->serialize = zend_class_serialize_deny;
	spl_ce_SplFileInfo->unserialize = zend_class_unserialize_deny;

	REGISTER_SPL_SUB_CLASS_EX(DirectoryIterator, SplFileInfo, spl_filesystem_object_new, spl_DirectoryIterator_functions);
	zend_class_implements(spl_ce_DirectoryIterator TSRMLS_CC, 1, zend_ce_iterator);
	REGISTER_SPL_IMPLEMENTS(DirectoryIterator, SeekableIterator);

	spl_ce_DirectoryIterator->get_iterator = spl_filesystem_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(FilesystemIterator, DirectoryIterator, spl_filesystem_object_new, spl_FilesystemIterator_functions);

	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_MODE_MASK",   SPL_FILE_DIR_CURRENT_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_PATHNAME", SPL_FILE_DIR_CURRENT_AS_PATHNAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_FILEINFO", SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_SELF",     SPL_FILE_DIR_CURRENT_AS_SELF);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_MODE_MASK",       SPL_FILE_DIR_KEY_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_PATHNAME",     SPL_FILE_DIR_KEY_AS_PATHNAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "FOLLOW_SYMLINKS",     SPL_FILE_DIR_FOLLOW_SYMLINKS);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_FILENAME",     SPL_FILE_DIR_KEY_AS_FILENAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "NEW_CURRENT_AND_KEY", SPL_FILE_DIR_KEY_AS_FILENAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "OTHER_MODE_MASK",     SPL_FILE_DIR_OTHERS_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "SKIP_DOTS",           SPL_FILE_DIR_SKIPDOTS);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "UNIX_PATHS",          SPL_FILE_DIR_UNIXPATHS);

	spl_ce_FilesystemIterator->get_iterator = spl_filesystem_tree_get_iterator;
 
 	REGISTER_SPL_SUB_CLASS_EX(RecursiveDirectoryIterator, FilesystemIterator, spl_filesystem_object_new, spl_RecursiveDirectoryIterator_functions);
 	REGISTER_SPL_IMPLEMENTS(RecursiveDirectoryIterator, RecursiveIterator);
 	memcpy(&spl_filesystem_object_check_handlers, &spl_filesystem_object_handlers, sizeof(zend_object_handlers));
 	spl_filesystem_object_check_handlers.get_method = spl_filesystem_object_get_method_check;
 
#ifdef HAVE_GLOB
	REGISTER_SPL_SUB_CLASS_EX(GlobIterator, FilesystemIterator, spl_filesystem_object_new_check, spl_GlobIterator_functions);
	REGISTER_SPL_IMPLEMENTS(GlobIterator, Countable);
#endif

	REGISTER_SPL_SUB_CLASS_EX(SplFileObject, SplFileInfo, spl_filesystem_object_new_check, spl_SplFileObject_functions);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, RecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, SeekableIterator);

	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "DROP_NEW_LINE", SPL_FILE_OBJECT_DROP_NEW_LINE);
 	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_AHEAD",    SPL_FILE_OBJECT_READ_AHEAD);
 	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "SKIP_EMPTY",    SPL_FILE_OBJECT_SKIP_EMPTY);
 	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_CSV",      SPL_FILE_OBJECT_READ_CSV);
 	REGISTER_SPL_SUB_CLASS_EX(SplTempFileObject, SplFileObject, spl_filesystem_object_new_check, spl_SplTempFileObject_functions);
 	return SUCCESS;
 }
