int exfat_mount(struct exfat* ef, const char* spec, const char* options)
{
	int rc;
	enum exfat_mode mode;

	exfat_tzset();
	memset(ef, 0, sizeof(struct exfat));

	parse_options(ef, options);

	if (match_option(options, "ro"))
		mode = EXFAT_MODE_RO;
	else if (match_option(options, "ro_fallback"))
		mode = EXFAT_MODE_ANY;
	else
		mode = EXFAT_MODE_RW;
	ef->dev = exfat_open(spec, mode);
	if (ef->dev == NULL)
		return -EIO;
	if (exfat_get_mode(ef->dev) == EXFAT_MODE_RO)
	{
		if (mode == EXFAT_MODE_ANY)
			ef->ro = -1;
		else
			ef->ro = 1;
	}

	ef->sb = malloc(sizeof(struct exfat_super_block));
	if (ef->sb == NULL)
	{
		exfat_close(ef->dev);
		exfat_error("failed to allocate memory for the super block");
		return -ENOMEM;
	}
	memset(ef->sb, 0, sizeof(struct exfat_super_block));

	if (exfat_pread(ef->dev, ef->sb, sizeof(struct exfat_super_block), 0) < 0)
	{
		exfat_close(ef->dev);
		free(ef->sb);
		exfat_error("failed to read boot sector");
		return -EIO;
	}
	if (memcmp(ef->sb->oem_name, "EXFAT   ", 8) != 0)
	{
		exfat_close(ef->dev);
		free(ef->sb);
 		exfat_error("exFAT file system is not found");
 		return -EIO;
 	}
	/* sector cannot be smaller than 512 bytes */
	if (ef->sb->sector_bits < 9)
	{
		exfat_close(ef->dev);
		exfat_error("too small sector size: 2^%hhd", ef->sb->sector_bits);
		free(ef->sb);
		return -EIO;
	}
	/* officially exFAT supports cluster size up to 32 MB */
	if ((int) ef->sb->sector_bits + (int) ef->sb->spc_bits > 25)
	{
		exfat_close(ef->dev);
		exfat_error("too big cluster size: 2^(%hhd+%hhd)",
				ef->sb->sector_bits, ef->sb->spc_bits);
		free(ef->sb);
		return -EIO;
	}
 	ef->zero_cluster = malloc(CLUSTER_SIZE(*ef->sb));
 	if (ef->zero_cluster == NULL)
 	{
		exfat_close(ef->dev);
		free(ef->sb);
		exfat_error("failed to allocate zero sector");
		return -ENOMEM;
	}
	/* use zero_cluster as a temporary buffer for VBR checksum verification */
	if (!verify_vbr_checksum(ef->dev, ef->zero_cluster, SECTOR_SIZE(*ef->sb)))
	{
		free(ef->zero_cluster);
		exfat_close(ef->dev);
		free(ef->sb);
		return -EIO;
	}
	memset(ef->zero_cluster, 0, CLUSTER_SIZE(*ef->sb));
	if (ef->sb->version.major != 1 || ef->sb->version.minor != 0)
	{
		free(ef->zero_cluster);
		exfat_close(ef->dev);
		exfat_error("unsupported exFAT version: %hhu.%hhu",
				ef->sb->version.major, ef->sb->version.minor);
		free(ef->sb);
		return -EIO;
	}
	if (ef->sb->fat_count != 1)
	{
		free(ef->zero_cluster);
		exfat_close(ef->dev);
		exfat_error("unsupported FAT count: %hhu", ef->sb->fat_count);
 		free(ef->sb);
 		return -EIO;
 	}
 	if (le64_to_cpu(ef->sb->sector_count) * SECTOR_SIZE(*ef->sb) >
 			exfat_get_size(ef->dev))
 	{
		/* this can cause I/O errors later but we don't fail mounting to let
		   user rescue data */
		exfat_warn("file system is larger than underlying device: "
				"%"PRIu64" > %"PRIu64,
				le64_to_cpu(ef->sb->sector_count) * SECTOR_SIZE(*ef->sb),
				exfat_get_size(ef->dev));
	}

	ef->root = malloc(sizeof(struct exfat_node));
	if (ef->root == NULL)
	{
		free(ef->zero_cluster);
		exfat_close(ef->dev);
		free(ef->sb);
		exfat_error("failed to allocate root node");
		return -ENOMEM;
	}
	memset(ef->root, 0, sizeof(struct exfat_node));
	ef->root->flags = EXFAT_ATTRIB_DIR;
	ef->root->start_cluster = le32_to_cpu(ef->sb->rootdir_cluster);
	ef->root->fptr_cluster = ef->root->start_cluster;
	ef->root->name[0] = cpu_to_le16('\0');
	ef->root->size = rootdir_size(ef);
	if (ef->root->size == 0)
	{
		free(ef->root);
		free(ef->zero_cluster);
		exfat_close(ef->dev);
		free(ef->sb);
		return -EIO;
	}
	/* exFAT does not have time attributes for the root directory */
	ef->root->mtime = 0;
	ef->root->atime = 0;
	/* always keep at least 1 reference to the root node */
	exfat_get_node(ef->root);

	rc = exfat_cache_directory(ef, ef->root);
	if (rc != 0)
		goto error;
	if (ef->upcase == NULL)
	{
		exfat_error("upcase table is not found");
		goto error;
	}
	if (ef->cmap.chunk == NULL)
	{
		exfat_error("clusters bitmap is not found");
		goto error;
	}

	if (prepare_super_block(ef) != 0)
		goto error;

	return 0;

error:
	exfat_put_node(ef, ef->root);
	exfat_reset_cache(ef);
	free(ef->root);
	free(ef->zero_cluster);
	exfat_close(ef->dev);
	free(ef->sb);
	return -EIO;
}
