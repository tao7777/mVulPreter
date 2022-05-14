 static int save_dev(blkid_dev dev, FILE *file)
 {
 	struct list_head *p;

	if (!dev || dev->bid_name[0] != '/')
		return 0;

	DBG(SAVE, ul_debug("device %s, type %s", dev->bid_name, dev->bid_type ?
		   dev->bid_type : "(null)"));

	fprintf(file, "<device DEVNO=\"0x%04lx\" TIME=\"%ld.%ld\"",
			(unsigned long) dev->bid_devno,
			(long) dev->bid_time,
			(long) dev->bid_utime);
 
 	if (dev->bid_pri)
 		fprintf(file, " PRI=\"%d\"", dev->bid_pri);
 	list_for_each(p, &dev->bid_tags) {
 		blkid_tag tag = list_entry(p, struct blkid_struct_tag, bit_tags);
		fprintf(file, " %s=\"%s\"", tag->bit_name,tag->bit_val);
 	}
 	fprintf(file, ">%s</device>\n", dev->bid_name);
 
	return 0;
}
