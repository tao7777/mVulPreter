 void grubfs_free (GrubFS *gf) {
 	if (gf) {
		if (gf->file && gf->file->device) {
 			free (gf->file->device->disk);
		}
 		free (gf->file);
 		free (gf);
	}
}
