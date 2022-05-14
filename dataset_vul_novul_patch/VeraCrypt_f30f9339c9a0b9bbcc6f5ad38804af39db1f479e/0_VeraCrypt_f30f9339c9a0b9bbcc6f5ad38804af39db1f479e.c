NTSTATUS ProcessMainDeviceControlIrp (PDEVICE_OBJECT DeviceObject, PEXTENSION Extension, PIRP Irp)
{
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (Irp);
	NTSTATUS ntStatus;

	switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
	{
	case TC_IOCTL_GET_DRIVER_VERSION:
	case TC_IOCTL_LEGACY_GET_DRIVER_VERSION:
		if (ValidateIOBufferSize (Irp, sizeof (LONG), ValidateOutput))
		{
			LONG tmp = VERSION_NUM;
			memcpy (Irp->AssociatedIrp.SystemBuffer, &tmp, 4);
			Irp->IoStatus.Information = sizeof (LONG);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_GET_DEVICE_REFCOUNT:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			*(int *) Irp->AssociatedIrp.SystemBuffer = DeviceObject->ReferenceCount;
			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_IS_DRIVER_UNLOAD_DISABLED:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			LONG deviceObjectCount = 0;

			*(int *) Irp->AssociatedIrp.SystemBuffer = DriverUnloadDisabled;

			if (IoEnumerateDeviceObjectList (TCDriverObject, NULL, 0, &deviceObjectCount) == STATUS_BUFFER_TOO_SMALL && deviceObjectCount > 1)
				*(int *) Irp->AssociatedIrp.SystemBuffer = TRUE;

			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_IS_ANY_VOLUME_MOUNTED:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			int drive;
			*(int *) Irp->AssociatedIrp.SystemBuffer = 0;

			for (drive = MIN_MOUNTED_VOLUME_DRIVE_NUMBER; drive <= MAX_MOUNTED_VOLUME_DRIVE_NUMBER; ++drive)
			{
				if (GetVirtualVolumeDeviceObject (drive))
				{
					*(int *) Irp->AssociatedIrp.SystemBuffer = 1;
					break;
				}
			}

			if (IsBootDriveMounted())
				*(int *) Irp->AssociatedIrp.SystemBuffer = 1;

			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_OPEN_TEST:
		{
			OPEN_TEST_STRUCT *opentest = (OPEN_TEST_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			OBJECT_ATTRIBUTES ObjectAttributes;
			HANDLE NtFileHandle;
			UNICODE_STRING FullFileName;
			IO_STATUS_BLOCK IoStatus;
			LARGE_INTEGER offset;
			ACCESS_MASK access = FILE_READ_ATTRIBUTES;

			if (!ValidateIOBufferSize (Irp, sizeof (OPEN_TEST_STRUCT), ValidateInputOutput))
				break;

			EnsureNullTerminatedString (opentest->wszFileName, sizeof (opentest->wszFileName));
			RtlInitUnicodeString (&FullFileName, opentest->wszFileName);

			InitializeObjectAttributes (&ObjectAttributes, &FullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

			if (opentest->bDetectTCBootLoader || opentest->DetectFilesystem || opentest->bComputeVolumeIDs)
				access |= FILE_READ_DATA;

			ntStatus = ZwCreateFile (&NtFileHandle,
						 SYNCHRONIZE | access, &ObjectAttributes, &IoStatus, NULL,
						 0, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

			if (NT_SUCCESS (ntStatus))
			{
				opentest->TCBootLoaderDetected = FALSE;
				opentest->FilesystemDetected = FALSE;
				memset (opentest->VolumeIDComputed, 0, sizeof (opentest->VolumeIDComputed));
				memset (opentest->volumeIDs, 0, sizeof (opentest->volumeIDs));

				if (opentest->bDetectTCBootLoader || opentest->DetectFilesystem || opentest->bComputeVolumeIDs)
				{
					byte *readBuffer = TCalloc (TC_MAX_VOLUME_SECTOR_SIZE);
					if (!readBuffer)
					{
						ntStatus = STATUS_INSUFFICIENT_RESOURCES;
					}
					else
					{
						if (opentest->bDetectTCBootLoader || opentest->DetectFilesystem)
						{

							offset.QuadPart = 0;

							ntStatus = ZwReadFile (NtFileHandle,
								NULL,
								NULL,
								NULL,
								&IoStatus,
								readBuffer,
								TC_MAX_VOLUME_SECTOR_SIZE,
								&offset,
								NULL);

							if (NT_SUCCESS (ntStatus))
							{
								size_t i;

								if (opentest->bDetectTCBootLoader && IoStatus.Information >= TC_SECTOR_SIZE_BIOS)
								{
									for (i = 0; i < TC_SECTOR_SIZE_BIOS - strlen (TC_APP_NAME); ++i)
									{
										if (memcmp (readBuffer + i, TC_APP_NAME, strlen (TC_APP_NAME)) == 0)
										{
											opentest->TCBootLoaderDetected = TRUE;
											break;
										}
									}
								}

								if (opentest->DetectFilesystem && IoStatus.Information >= sizeof (int64))
								{
									switch (BE64 (*(uint64 *) readBuffer))
									{
									case 0xEB52904E54465320ULL: // NTFS
									case 0xEB3C904D53444F53ULL: // FAT16/FAT32
									case 0xEB58904D53444F53ULL: // FAT32
									case 0xEB76904558464154ULL: // exFAT
									case 0x0000005265465300ULL: // ReFS
									case 0xEB58906D6B66732EULL: // FAT32 mkfs.fat
									case 0xEB58906D6B646F73ULL: // FAT32 mkfs.vfat/mkdosfs
									case 0xEB3C906D6B66732EULL: // FAT16/FAT12 mkfs.fat
									case 0xEB3C906D6B646F73ULL: // FAT16/FAT12 mkfs.vfat/mkdosfs
										opentest->FilesystemDetected = TRUE;
										break;
									case 0x0000000000000000ULL:
										if (IsAllZeroes (readBuffer + 8, TC_VOLUME_HEADER_EFFECTIVE_SIZE - 8))
											opentest->FilesystemDetected = TRUE;
										break;
									}
								}
							}
						}

						if (opentest->bComputeVolumeIDs && (!opentest->DetectFilesystem || !opentest->FilesystemDetected))
						{
							int volumeType;
							for (volumeType = TC_VOLUME_TYPE_NORMAL;
								volumeType < TC_VOLUME_TYPE_COUNT;
								volumeType++)
							{
								/* Read the volume header */
								switch (volumeType)
								{
								case TC_VOLUME_TYPE_NORMAL:
									offset.QuadPart = TC_VOLUME_HEADER_OFFSET;
									break;

								case TC_VOLUME_TYPE_HIDDEN:

									offset.QuadPart = TC_HIDDEN_VOLUME_HEADER_OFFSET;
									break;
								}

								ntStatus = ZwReadFile (NtFileHandle,
								NULL,
								NULL,
								NULL,
								&IoStatus,
								readBuffer,
								TC_MAX_VOLUME_SECTOR_SIZE,
								&offset,
								NULL);

								if (NT_SUCCESS (ntStatus))
								{
									/* compute the ID of this volume: SHA-256 of the effective header */
									sha256 (opentest->volumeIDs[volumeType], readBuffer, TC_VOLUME_HEADER_EFFECTIVE_SIZE);
									opentest->VolumeIDComputed[volumeType] = TRUE;
								}
							}
						}

						TCfree (readBuffer);
					}
				}

				ZwClose (NtFileHandle);
				Dump ("Open test on file %ls success.\n", opentest->wszFileName);
			}
			else
			{
#if 0
				Dump ("Open test on file %ls failed NTSTATUS 0x%08x\n", opentest->wszFileName, ntStatus);
#endif
			}

			Irp->IoStatus.Information = NT_SUCCESS (ntStatus) ? sizeof (OPEN_TEST_STRUCT) : 0;
			Irp->IoStatus.Status = ntStatus;
		}
		break;

	case TC_IOCTL_GET_SYSTEM_DRIVE_CONFIG:
		{
			GetSystemDriveConfigurationRequest *request = (GetSystemDriveConfigurationRequest *) Irp->AssociatedIrp.SystemBuffer;
			OBJECT_ATTRIBUTES ObjectAttributes;
			HANDLE NtFileHandle;
 			UNICODE_STRING FullFileName;
 			IO_STATUS_BLOCK IoStatus;
 			LARGE_INTEGER offset;
			size_t devicePathLen = 0;
 
 			if (!ValidateIOBufferSize (Irp, sizeof (GetSystemDriveConfigurationRequest), ValidateInputOutput))
 				break;
 
			// check that request->DevicePath has the expected format "\\Device\\HarddiskXXX\\Partition0"
			if (	!NT_SUCCESS (RtlUnalignedStringCchLengthW (request->DevicePath, TC_MAX_PATH, &devicePathLen))
				||	(devicePathLen < 28) // 28 is the length of "\\Device\\Harddisk0\\Partition0" which is the minimum
				||	(devicePathLen > 30) // 30 is the length of "\\Device\\Harddisk255\\Partition0" which is the maximum
				||	(memcmp (request->DevicePath, L"\\Device\\Harddisk", 16 * sizeof (WCHAR)))
				||	(memcmp (&request->DevicePath[devicePathLen - 11], L"\\Partition0", 11 * sizeof (WCHAR)))
				)
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
				break;
			}

 			EnsureNullTerminatedString (request->DevicePath, sizeof (request->DevicePath));
 			RtlInitUnicodeString (&FullFileName, request->DevicePath);
 
			InitializeObjectAttributes (&ObjectAttributes, &FullFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

			ntStatus = ZwCreateFile (&NtFileHandle,
				SYNCHRONIZE | GENERIC_READ, &ObjectAttributes, &IoStatus, NULL,
				FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_RANDOM_ACCESS, NULL, 0);
 
 			if (NT_SUCCESS (ntStatus))
 			{
				byte *readBuffer = TCalloc (TC_MAX_VOLUME_SECTOR_SIZE);
				if (!readBuffer)
 				{
					Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					Irp->IoStatus.Information = 0;
				}
				else
				{
					// Determine if the first sector contains a portion of the VeraCrypt Boot Loader
					offset.QuadPart = 0;	// MBR
 
					ntStatus = ZwReadFile (NtFileHandle,
						NULL,
						NULL,
						NULL,
						&IoStatus,
						readBuffer,
						TC_MAX_VOLUME_SECTOR_SIZE,
						&offset,
						NULL);
 
					if (NT_SUCCESS (ntStatus))
 					{
						// check that we could read all needed data
						if (IoStatus.Information >= TC_SECTOR_SIZE_BIOS)
 						{
							size_t i;

							// Check for dynamic drive
							request->DriveIsDynamic = FALSE;

							if (readBuffer[510] == 0x55 && readBuffer[511] == 0xaa)
 							{
								int i;
								for (i = 0; i < 4; ++i)
								{
									if (readBuffer[446 + i * 16 + 4] == PARTITION_LDM)
									{
										request->DriveIsDynamic = TRUE;
										break;
									}
								}
 							}
 
							request->BootLoaderVersion = 0;
							request->Configuration = 0;
							request->UserConfiguration = 0;
							request->CustomUserMessage[0] = 0;
 
							// Search for the string "VeraCrypt"
							for (i = 0; i < sizeof (readBuffer) - strlen (TC_APP_NAME); ++i)
 							{
								if (memcmp (readBuffer + i, TC_APP_NAME, strlen (TC_APP_NAME)) == 0)
								{
									request->BootLoaderVersion = BE16 (*(uint16 *) (readBuffer + TC_BOOT_SECTOR_VERSION_OFFSET));
									request->Configuration = readBuffer[TC_BOOT_SECTOR_CONFIG_OFFSET];

									if (request->BootLoaderVersion != 0 && request->BootLoaderVersion <= VERSION_NUM)
									{
										request->UserConfiguration = readBuffer[TC_BOOT_SECTOR_USER_CONFIG_OFFSET];
										memcpy (request->CustomUserMessage, readBuffer + TC_BOOT_SECTOR_USER_MESSAGE_OFFSET, TC_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH);
									}
									break;
								}
 							}

							Irp->IoStatus.Status = STATUS_SUCCESS;
							Irp->IoStatus.Information = sizeof (*request);
						}
						else
						{
							Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
							Irp->IoStatus.Information = 0;
 						}
 					}
					else
					{
						Irp->IoStatus.Status = ntStatus;
						Irp->IoStatus.Information = 0;
					}
 
					TCfree (readBuffer);
 				}
 
 				ZwClose (NtFileHandle);

			}
			else
			{
				Irp->IoStatus.Status = ntStatus;
				Irp->IoStatus.Information = 0;
			}
		}
		break;

	case TC_IOCTL_WIPE_PASSWORD_CACHE:
		WipeCache ();

		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_PASSWORD_CACHE_STATUS:
		Irp->IoStatus.Status = cacheEmpty ? STATUS_PIPE_EMPTY : STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_SET_PORTABLE_MODE_STATUS:
		if (!UserCanAccessDriveDevice())
		{
			Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
			Irp->IoStatus.Information = 0;
		}
		else
		{
			PortableMode = TRUE;
			Dump ("Setting portable mode\n");
		}
		break;

	case TC_IOCTL_GET_PORTABLE_MODE_STATUS:
		Irp->IoStatus.Status = PortableMode ? STATUS_SUCCESS : STATUS_PIPE_EMPTY;
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_MOUNTED_VOLUMES:

		if (ValidateIOBufferSize (Irp, sizeof (MOUNT_LIST_STRUCT), ValidateOutput))
		{
			MOUNT_LIST_STRUCT *list = (MOUNT_LIST_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice;
			int drive;

			list->ulMountedDrives = 0;

			for (drive = MIN_MOUNTED_VOLUME_DRIVE_NUMBER; drive <= MAX_MOUNTED_VOLUME_DRIVE_NUMBER; ++drive)
			{
				PEXTENSION ListExtension;

				ListDevice = GetVirtualVolumeDeviceObject (drive);
				if (!ListDevice)
					continue;

				ListExtension = (PEXTENSION) ListDevice->DeviceExtension;
				if (IsVolumeAccessibleByCurrentUser (ListExtension))
				{
					list->ulMountedDrives |= (1 << ListExtension->nDosDriveNo);
					RtlStringCbCopyW (list->wszVolume[ListExtension->nDosDriveNo], sizeof(list->wszVolume[ListExtension->nDosDriveNo]),ListExtension->wszVolume);
					RtlStringCbCopyW (list->wszLabel[ListExtension->nDosDriveNo], sizeof(list->wszLabel[ListExtension->nDosDriveNo]),ListExtension->wszLabel);
					memcpy (list->volumeID[ListExtension->nDosDriveNo], ListExtension->volumeID, VOLUME_ID_SIZE);
					list->diskLength[ListExtension->nDosDriveNo] = ListExtension->DiskLength;
					list->ea[ListExtension->nDosDriveNo] = ListExtension->cryptoInfo->ea;
					if (ListExtension->cryptoInfo->hiddenVolume)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_HIDDEN;	// Hidden volume
					else if (ListExtension->cryptoInfo->bHiddenVolProtectionAction)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_OUTER_VOL_WRITE_PREVENTED;	// Normal/outer volume (hidden volume protected AND write already prevented)
					else if (ListExtension->cryptoInfo->bProtectHiddenVolume)
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_OUTER;	// Normal/outer volume (hidden volume protected)
					else
						list->volumeType[ListExtension->nDosDriveNo] = PROP_VOL_TYPE_NORMAL;	// Normal volume
					list->truecryptMode[ListExtension->nDosDriveNo] = ListExtension->cryptoInfo->bTrueCryptMode;
				}
			}

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof (MOUNT_LIST_STRUCT);
		}
		break;

	case TC_IOCTL_LEGACY_GET_MOUNTED_VOLUMES:
		if (ValidateIOBufferSize (Irp, sizeof (uint32), ValidateOutput))
		{

			memset (Irp->AssociatedIrp.SystemBuffer, 0, irpSp->Parameters.DeviceIoControl.OutputBufferLength);
			*(uint32 *) Irp->AssociatedIrp.SystemBuffer = 0xffffFFFF;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
		}
		break;

	case TC_IOCTL_GET_VOLUME_PROPERTIES:
		if (ValidateIOBufferSize (Irp, sizeof (VOLUME_PROPERTIES_STRUCT), ValidateInputOutput))
		{
			VOLUME_PROPERTIES_STRUCT *prop = (VOLUME_PROPERTIES_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice = GetVirtualVolumeDeviceObject (prop->driveNo);

			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;

			if (ListDevice)
			{
				PEXTENSION ListExtension = (PEXTENSION) ListDevice->DeviceExtension;
				if (IsVolumeAccessibleByCurrentUser (ListExtension))
				{
					prop->uniqueId = ListExtension->UniqueVolumeId;
					RtlStringCbCopyW (prop->wszVolume, sizeof(prop->wszVolume),ListExtension->wszVolume);
					RtlStringCbCopyW (prop->wszLabel, sizeof(prop->wszLabel),ListExtension->wszLabel);
					memcpy (prop->volumeID, ListExtension->volumeID, VOLUME_ID_SIZE);
					prop->bDriverSetLabel = ListExtension->bDriverSetLabel;
					prop->diskLength = ListExtension->DiskLength;
					prop->ea = ListExtension->cryptoInfo->ea;
					prop->mode = ListExtension->cryptoInfo->mode;
					prop->pkcs5 = ListExtension->cryptoInfo->pkcs5;
					prop->pkcs5Iterations = ListExtension->cryptoInfo->noIterations;
					prop->volumePim = ListExtension->cryptoInfo->volumePim;
#if 0
					prop->volumeCreationTime = ListExtension->cryptoInfo->volume_creation_time;
					prop->headerCreationTime = ListExtension->cryptoInfo->header_creation_time;
#endif
					prop->volumeHeaderFlags = ListExtension->cryptoInfo->HeaderFlags;
					prop->readOnly = ListExtension->bReadOnly;
					prop->removable = ListExtension->bRemovable;
					prop->partitionInInactiveSysEncScope = ListExtension->PartitionInInactiveSysEncScope;
					prop->hiddenVolume = ListExtension->cryptoInfo->hiddenVolume;

					if (ListExtension->cryptoInfo->bProtectHiddenVolume)
						prop->hiddenVolProtection = ListExtension->cryptoInfo->bHiddenVolProtectionAction ? HIDVOL_PROT_STATUS_ACTION_TAKEN : HIDVOL_PROT_STATUS_ACTIVE;
					else
						prop->hiddenVolProtection = HIDVOL_PROT_STATUS_NONE;

					prop->totalBytesRead = ListExtension->Queue.TotalBytesRead;
					prop->totalBytesWritten = ListExtension->Queue.TotalBytesWritten;

					prop->volFormatVersion = ListExtension->cryptoInfo->LegacyVolume ? TC_VOLUME_FORMAT_VERSION_PRE_6_0 : TC_VOLUME_FORMAT_VERSION;

					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof (VOLUME_PROPERTIES_STRUCT);
				}
			}
		}
		break;

	case TC_IOCTL_GET_RESOLVED_SYMLINK:
		if (ValidateIOBufferSize (Irp, sizeof (RESOLVE_SYMLINK_STRUCT), ValidateInputOutput))
		{
			RESOLVE_SYMLINK_STRUCT *resolve = (RESOLVE_SYMLINK_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (resolve->symLinkName, sizeof (resolve->symLinkName));

				ntStatus = SymbolicLinkToTarget (resolve->symLinkName,
					resolve->targetName,
					sizeof (resolve->targetName));

				Irp->IoStatus.Information = sizeof (RESOLVE_SYMLINK_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case TC_IOCTL_GET_DRIVE_PARTITION_INFO:
		if (ValidateIOBufferSize (Irp, sizeof (DISK_PARTITION_INFO_STRUCT), ValidateInputOutput))
		{
			DISK_PARTITION_INFO_STRUCT *info = (DISK_PARTITION_INFO_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				PARTITION_INFORMATION_EX pi;
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (info->deviceName, sizeof (info->deviceName));

				ntStatus = TCDeviceIoControl (info->deviceName, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0, &pi, sizeof (pi));
				if (NT_SUCCESS(ntStatus))
				{
					memset (&info->partInfo, 0, sizeof (info->partInfo));

					info->partInfo.PartitionLength = pi.PartitionLength;
					info->partInfo.PartitionNumber = pi.PartitionNumber;
					info->partInfo.StartingOffset = pi.StartingOffset;

					if (pi.PartitionStyle == PARTITION_STYLE_MBR)
					{
						info->partInfo.PartitionType = pi.Mbr.PartitionType;
						info->partInfo.BootIndicator = pi.Mbr.BootIndicator;
					}

					info->IsGPT = pi.PartitionStyle == PARTITION_STYLE_GPT;
				}
				else
				{
					ntStatus = TCDeviceIoControl (info->deviceName, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &info->partInfo, sizeof (info->partInfo));
					info->IsGPT = FALSE;
				}

				if (!NT_SUCCESS (ntStatus))
				{
					GET_LENGTH_INFORMATION lengthInfo;
					ntStatus = TCDeviceIoControl (info->deviceName, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof (lengthInfo));

					if (NT_SUCCESS (ntStatus))
					{
						memset (&info->partInfo, 0, sizeof (info->partInfo));
						info->partInfo.PartitionLength = lengthInfo.Length;
					}
				}

				info->IsDynamic = FALSE;

				if (NT_SUCCESS (ntStatus) && OsMajorVersion >= 6)
				{
#					define IOCTL_VOLUME_IS_DYNAMIC CTL_CODE(IOCTL_VOLUME_BASE, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
					if (!NT_SUCCESS (TCDeviceIoControl (info->deviceName, IOCTL_VOLUME_IS_DYNAMIC, NULL, 0, &info->IsDynamic, sizeof (info->IsDynamic))))
						info->IsDynamic = FALSE;
				}

				Irp->IoStatus.Information = sizeof (DISK_PARTITION_INFO_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case TC_IOCTL_GET_DRIVE_GEOMETRY:
		if (ValidateIOBufferSize (Irp, sizeof (DISK_GEOMETRY_STRUCT), ValidateInputOutput))
		{
			DISK_GEOMETRY_STRUCT *g = (DISK_GEOMETRY_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				NTSTATUS ntStatus;

				EnsureNullTerminatedString (g->deviceName, sizeof (g->deviceName));
				Dump ("Calling IOCTL_DISK_GET_DRIVE_GEOMETRY on %ls\n", g->deviceName);

				ntStatus = TCDeviceIoControl (g->deviceName,
					IOCTL_DISK_GET_DRIVE_GEOMETRY,
					NULL, 0, &g->diskGeometry, sizeof (g->diskGeometry));

				Irp->IoStatus.Information = sizeof (DISK_GEOMETRY_STRUCT);
				Irp->IoStatus.Status = ntStatus;
			}
		}
		break;

	case VC_IOCTL_GET_DRIVE_GEOMETRY_EX:
		if (ValidateIOBufferSize (Irp, sizeof (DISK_GEOMETRY_EX_STRUCT), ValidateInputOutput))
		{
			DISK_GEOMETRY_EX_STRUCT *g = (DISK_GEOMETRY_EX_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			{
				NTSTATUS ntStatus;
				PVOID buffer = TCalloc (256); // enough for DISK_GEOMETRY_EX and padded data
				if (buffer)
				{
					EnsureNullTerminatedString (g->deviceName, sizeof (g->deviceName));
					Dump ("Calling IOCTL_DISK_GET_DRIVE_GEOMETRY_EX on %ls\n", g->deviceName);

					ntStatus = TCDeviceIoControl (g->deviceName,
						IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
						NULL, 0, buffer, 256);

					if (NT_SUCCESS(ntStatus))
					{
						PDISK_GEOMETRY_EX pGeo = (PDISK_GEOMETRY_EX) buffer;
						memcpy (&g->diskGeometry, &pGeo->Geometry, sizeof (DISK_GEOMETRY));
						g->DiskSize.QuadPart = pGeo->DiskSize.QuadPart;
					}
					else
					{
						DISK_GEOMETRY dg = {0};
						Dump ("Failed. Calling IOCTL_DISK_GET_DRIVE_GEOMETRY on %ls\n", g->deviceName);
						ntStatus = TCDeviceIoControl (g->deviceName,
							IOCTL_DISK_GET_DRIVE_GEOMETRY,
							NULL, 0, &dg, sizeof (dg));

						if (NT_SUCCESS(ntStatus))
						{
							memcpy (&g->diskGeometry, &dg, sizeof (DISK_GEOMETRY));
							g->DiskSize.QuadPart = dg.Cylinders.QuadPart * dg.SectorsPerTrack * dg.TracksPerCylinder * dg.BytesPerSector;

							if (OsMajorVersion >= 6)
							{
								STORAGE_READ_CAPACITY storage = {0};
								NTSTATUS lStatus;
								storage.Version = sizeof (STORAGE_READ_CAPACITY);
								Dump ("Calling IOCTL_STORAGE_READ_CAPACITY on %ls\n", g->deviceName);
								lStatus = TCDeviceIoControl (g->deviceName,
									IOCTL_STORAGE_READ_CAPACITY,
									NULL, 0, &storage, sizeof (STORAGE_READ_CAPACITY));
								if (	NT_SUCCESS(lStatus)
									&& (storage.Size == sizeof (STORAGE_READ_CAPACITY))
									)
								{
									g->DiskSize.QuadPart = storage.DiskLength.QuadPart;
								}
							}
						}
					}

					TCfree (buffer);

					Irp->IoStatus.Information = sizeof (DISK_GEOMETRY_EX_STRUCT);
					Irp->IoStatus.Status = ntStatus;
				}
				else
				{
					Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					Irp->IoStatus.Information = 0;
				}
			}
		}
		break;

	case TC_IOCTL_PROBE_REAL_DRIVE_SIZE:
		if (ValidateIOBufferSize (Irp, sizeof (ProbeRealDriveSizeRequest), ValidateInputOutput))
		{
			ProbeRealDriveSizeRequest *request = (ProbeRealDriveSizeRequest *) Irp->AssociatedIrp.SystemBuffer;
			NTSTATUS status;
			UNICODE_STRING name;
			PFILE_OBJECT fileObject;
			PDEVICE_OBJECT deviceObject;

			EnsureNullTerminatedString (request->DeviceName, sizeof (request->DeviceName));

			RtlInitUnicodeString (&name, request->DeviceName);
			status = IoGetDeviceObjectPointer (&name, FILE_READ_ATTRIBUTES, &fileObject, &deviceObject);
			if (!NT_SUCCESS (status))
			{
				Irp->IoStatus.Information = 0;
				Irp->IoStatus.Status = status;
				break;
			}

			status = ProbeRealDriveSize (deviceObject, &request->RealDriveSize);
			ObDereferenceObject (fileObject);

			if (status == STATUS_TIMEOUT)
			{
				request->TimeOut = TRUE;
				Irp->IoStatus.Information = sizeof (ProbeRealDriveSizeRequest);
				Irp->IoStatus.Status = STATUS_SUCCESS;
			}
			else if (!NT_SUCCESS (status))
			{
				Irp->IoStatus.Information = 0;
				Irp->IoStatus.Status = status;
			}
			else
			{
				request->TimeOut = FALSE;
				Irp->IoStatus.Information = sizeof (ProbeRealDriveSizeRequest);
				Irp->IoStatus.Status = status;
			}
		}
		break;

	case TC_IOCTL_MOUNT_VOLUME:
		if (ValidateIOBufferSize (Irp, sizeof (MOUNT_STRUCT), ValidateInputOutput))
		{
			MOUNT_STRUCT *mount = (MOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;

			if (mount->VolumePassword.Length > MAX_PASSWORD || mount->ProtectedHidVolPassword.Length > MAX_PASSWORD
				||	mount->pkcs5_prf < 0 || mount->pkcs5_prf > LAST_PRF_ID
				||	mount->VolumePim < -1 || mount->VolumePim == INT_MAX
				|| mount->ProtectedHidVolPkcs5Prf < 0 || mount->ProtectedHidVolPkcs5Prf > LAST_PRF_ID
				|| (mount->bTrueCryptMode != FALSE && mount->bTrueCryptMode != TRUE)
				)
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
				break;
			}

			EnsureNullTerminatedString (mount->wszVolume, sizeof (mount->wszVolume));
			EnsureNullTerminatedString (mount->wszLabel, sizeof (mount->wszLabel));

			Irp->IoStatus.Information = sizeof (MOUNT_STRUCT);
			Irp->IoStatus.Status = MountDevice (DeviceObject, mount);

			burn (&mount->VolumePassword, sizeof (mount->VolumePassword));
			burn (&mount->ProtectedHidVolPassword, sizeof (mount->ProtectedHidVolPassword));
			burn (&mount->pkcs5_prf, sizeof (mount->pkcs5_prf));
			burn (&mount->VolumePim, sizeof (mount->VolumePim));
			burn (&mount->bTrueCryptMode, sizeof (mount->bTrueCryptMode));
			burn (&mount->ProtectedHidVolPkcs5Prf, sizeof (mount->ProtectedHidVolPkcs5Prf));
			burn (&mount->ProtectedHidVolPim, sizeof (mount->ProtectedHidVolPim));
		}
		break;

	case TC_IOCTL_DISMOUNT_VOLUME:
		if (ValidateIOBufferSize (Irp, sizeof (UNMOUNT_STRUCT), ValidateInputOutput))
		{
			UNMOUNT_STRUCT *unmount = (UNMOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;
			PDEVICE_OBJECT ListDevice = GetVirtualVolumeDeviceObject (unmount->nDosDriveNo);

			unmount->nReturnCode = ERR_DRIVE_NOT_FOUND;

			if (ListDevice)
			{
				PEXTENSION ListExtension = (PEXTENSION) ListDevice->DeviceExtension;

				if (IsVolumeAccessibleByCurrentUser (ListExtension))
					unmount->nReturnCode = UnmountDevice (unmount, ListDevice, unmount->ignoreOpenFiles);
			}

			Irp->IoStatus.Information = sizeof (UNMOUNT_STRUCT);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_DISMOUNT_ALL_VOLUMES:
		if (ValidateIOBufferSize (Irp, sizeof (UNMOUNT_STRUCT), ValidateInputOutput))
		{
			UNMOUNT_STRUCT *unmount = (UNMOUNT_STRUCT *) Irp->AssociatedIrp.SystemBuffer;

			unmount->nReturnCode = UnmountAllDevices (unmount, unmount->ignoreOpenFiles);

			Irp->IoStatus.Information = sizeof (UNMOUNT_STRUCT);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_BOOT_ENCRYPTION_SETUP:
		Irp->IoStatus.Status = StartBootEncryptionSetup (DeviceObject, Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP:
		Irp->IoStatus.Status = AbortBootEncryptionSetup();
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_BOOT_ENCRYPTION_STATUS:
		GetBootEncryptionStatus (Irp, irpSp);
		break;

	case TC_IOCTL_GET_BOOT_ENCRYPTION_SETUP_RESULT:
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = GetSetupResult();
		break;

	case TC_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES:
		GetBootDriveVolumeProperties (Irp, irpSp);
		break;

	case TC_IOCTL_GET_BOOT_LOADER_VERSION:
		GetBootLoaderVersion (Irp, irpSp);
		break;

	case TC_IOCTL_REOPEN_BOOT_VOLUME_HEADER:
		ReopenBootVolumeHeader (Irp, irpSp);
		break;

	case VC_IOCTL_GET_BOOT_LOADER_FINGERPRINT:
		GetBootLoaderFingerprint (Irp, irpSp);
		break;

	case TC_IOCTL_GET_BOOT_ENCRYPTION_ALGORITHM_NAME:
		GetBootEncryptionAlgorithmName (Irp, irpSp);
		break;

	case TC_IOCTL_IS_HIDDEN_SYSTEM_RUNNING:
		if (ValidateIOBufferSize (Irp, sizeof (int), ValidateOutput))
		{
			*(int *) Irp->AssociatedIrp.SystemBuffer = IsHiddenSystemRunning() ? 1 : 0;
			Irp->IoStatus.Information = sizeof (int);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_START_DECOY_SYSTEM_WIPE:
		Irp->IoStatus.Status = StartDecoySystemWipe (DeviceObject, Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_ABORT_DECOY_SYSTEM_WIPE:
		Irp->IoStatus.Status = AbortDecoySystemWipe();
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_DECOY_SYSTEM_WIPE_RESULT:
		Irp->IoStatus.Status = GetDecoySystemWipeResult();
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS:
		GetDecoySystemWipeStatus (Irp, irpSp);
		break;

	case TC_IOCTL_WRITE_BOOT_DRIVE_SECTOR:
		Irp->IoStatus.Status = WriteBootDriveSector (Irp, irpSp);
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_WARNING_FLAGS:
		if (ValidateIOBufferSize (Irp, sizeof (GetWarningFlagsRequest), ValidateOutput))
		{
			GetWarningFlagsRequest *flags = (GetWarningFlagsRequest *) Irp->AssociatedIrp.SystemBuffer;

			flags->PagingFileCreationPrevented = PagingFileCreationPrevented;
			PagingFileCreationPrevented = FALSE;
			flags->SystemFavoriteVolumeDirty = SystemFavoriteVolumeDirty;
			SystemFavoriteVolumeDirty = FALSE;

			Irp->IoStatus.Information = sizeof (GetWarningFlagsRequest);
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		break;

	case TC_IOCTL_SET_SYSTEM_FAVORITE_VOLUME_DIRTY:
		if (UserCanAccessDriveDevice())
		{
			SystemFavoriteVolumeDirty = TRUE;
			Irp->IoStatus.Status = STATUS_SUCCESS;
		}
		else
			Irp->IoStatus.Status = STATUS_ACCESS_DENIED;

		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_REREAD_DRIVER_CONFIG:
		Irp->IoStatus.Status = ReadRegistryConfigFlags (FALSE);
		Irp->IoStatus.Information = 0;
		break;

	case TC_IOCTL_GET_SYSTEM_DRIVE_DUMP_CONFIG:
		if (	(ValidateIOBufferSize (Irp, sizeof (GetSystemDriveDumpConfigRequest), ValidateOutput))
			&&	(Irp->RequestorMode == KernelMode)
			)
		{
			GetSystemDriveDumpConfigRequest *request = (GetSystemDriveDumpConfigRequest *) Irp->AssociatedIrp.SystemBuffer;

			request->BootDriveFilterExtension = GetBootDriveFilterExtension();
			if (IsBootDriveMounted() && request->BootDriveFilterExtension)
			{
				request->HwEncryptionEnabled = IsHwEncryptionEnabled();
				Irp->IoStatus.Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = sizeof (*request);
			}
			else
			{
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
				Irp->IoStatus.Information = 0;
			}
		}
		break;

	default:
		return TCCompleteIrp (Irp, STATUS_INVALID_DEVICE_REQUEST, 0);
	}


#if defined(DEBUG) || defined(DEBUG_TRACE)
	if (!NT_SUCCESS (Irp->IoStatus.Status))
	{
		switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
		{
		case TC_IOCTL_GET_MOUNTED_VOLUMES:
		case TC_IOCTL_GET_PASSWORD_CACHE_STATUS:
		case TC_IOCTL_GET_PORTABLE_MODE_STATUS:
		case TC_IOCTL_SET_PORTABLE_MODE_STATUS:
		case TC_IOCTL_OPEN_TEST:
		case TC_IOCTL_GET_RESOLVED_SYMLINK:
		case TC_IOCTL_GET_DRIVE_PARTITION_INFO:
		case TC_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES:
		case TC_IOCTL_GET_BOOT_ENCRYPTION_STATUS:
		case TC_IOCTL_IS_HIDDEN_SYSTEM_RUNNING:
			break;

		default:
			Dump ("IOCTL error 0x%08x\n", Irp->IoStatus.Status);
		}
	}
#endif

	return TCCompleteIrp (Irp, Irp->IoStatus.Status, Irp->IoStatus.Information);
}
