EnumTraits<media::mojom::VideoCaptureError, media::VideoCaptureError>::ToMojom(
    media::VideoCaptureError input) {
  switch (input) {
    case media::VideoCaptureError::kNone:
      return media::mojom::VideoCaptureError::kNone;
    case media::VideoCaptureError::
        kVideoCaptureControllerInvalidOrUnsupportedVideoCaptureParametersRequested:
      return media::mojom::VideoCaptureError::
          kVideoCaptureControllerInvalidOrUnsupportedVideoCaptureParametersRequested;
    case media::VideoCaptureError::kVideoCaptureControllerIsAlreadyInErrorState:
      return media::mojom::VideoCaptureError::
          kVideoCaptureControllerIsAlreadyInErrorState;
    case media::VideoCaptureError::kVideoCaptureManagerDeviceConnectionLost:
      return media::mojom::VideoCaptureError::
          kVideoCaptureManagerDeviceConnectionLost;
    case media::VideoCaptureError::
        kFrameSinkVideoCaptureDeviceAleradyEndedOnFatalError:
      return media::mojom::VideoCaptureError::
          kFrameSinkVideoCaptureDeviceAleradyEndedOnFatalError;
    case media::VideoCaptureError::
        kFrameSinkVideoCaptureDeviceEncounteredFatalError:
      return media::mojom::VideoCaptureError::
          kFrameSinkVideoCaptureDeviceEncounteredFatalError;
    case media::VideoCaptureError::kV4L2FailedToOpenV4L2DeviceDriverFile:
      return media::mojom::VideoCaptureError::
          kV4L2FailedToOpenV4L2DeviceDriverFile;
    case media::VideoCaptureError::kV4L2ThisIsNotAV4L2VideoCaptureDevice:
      return media::mojom::VideoCaptureError::
          kV4L2ThisIsNotAV4L2VideoCaptureDevice;
    case media::VideoCaptureError::kV4L2FailedToFindASupportedCameraFormat:
      return media::mojom::VideoCaptureError::
          kV4L2FailedToFindASupportedCameraFormat;
    case media::VideoCaptureError::kV4L2FailedToSetVideoCaptureFormat:
      return media::mojom::VideoCaptureError::
          kV4L2FailedToSetVideoCaptureFormat;
    case media::VideoCaptureError::kV4L2UnsupportedPixelFormat:
      return media::mojom::VideoCaptureError::kV4L2UnsupportedPixelFormat;
    case media::VideoCaptureError::kV4L2FailedToSetCameraFramerate:
      return media::mojom::VideoCaptureError::kV4L2FailedToSetCameraFramerate;
    case media::VideoCaptureError::kV4L2ErrorRequestingMmapBuffers:
      return media::mojom::VideoCaptureError::kV4L2ErrorRequestingMmapBuffers;
    case media::VideoCaptureError::kV4L2AllocateBufferFailed:
      return media::mojom::VideoCaptureError::kV4L2AllocateBufferFailed;
    case media::VideoCaptureError::kV4L2VidiocStreamonFailed:
      return media::mojom::VideoCaptureError::kV4L2VidiocStreamonFailed;
    case media::VideoCaptureError::kV4L2VidiocStreamoffFailed:
      return media::mojom::VideoCaptureError::kV4L2VidiocStreamoffFailed;
    case media::VideoCaptureError::kV4L2FailedToVidiocReqbufsWithCount0:
      return media::mojom::VideoCaptureError::
          kV4L2FailedToVidiocReqbufsWithCount0;
    case media::VideoCaptureError::kV4L2PollFailed:
      return media::mojom::VideoCaptureError::kV4L2PollFailed;
    case media::VideoCaptureError::
        kV4L2MultipleContinuousTimeoutsWhileReadPolling:
      return media::mojom::VideoCaptureError::
          kV4L2MultipleContinuousTimeoutsWhileReadPolling;
    case media::VideoCaptureError::kV4L2FailedToDequeueCaptureBuffer:
      return media::mojom::VideoCaptureError::kV4L2FailedToDequeueCaptureBuffer;
    case media::VideoCaptureError::kV4L2FailedToEnqueueCaptureBuffer:
      return media::mojom::VideoCaptureError::kV4L2FailedToEnqueueCaptureBuffer;
    case media::VideoCaptureError::
        kSingleClientVideoCaptureHostLostConnectionToDevice:
      return media::mojom::VideoCaptureError::
          kSingleClientVideoCaptureHostLostConnectionToDevice;
    case media::VideoCaptureError::kSingleClientVideoCaptureDeviceLaunchAborted:
      return media::mojom::VideoCaptureError::
          kSingleClientVideoCaptureDeviceLaunchAborted;
    case media::VideoCaptureError::
        kDesktopCaptureDeviceWebrtcDesktopCapturerHasFailed:
      return media::mojom::VideoCaptureError::
          kDesktopCaptureDeviceWebrtcDesktopCapturerHasFailed;
    case media::VideoCaptureError::kFileVideoCaptureDeviceCouldNotOpenVideoFile:
      return media::mojom::VideoCaptureError::
          kFileVideoCaptureDeviceCouldNotOpenVideoFile;
    case media::VideoCaptureError::
        kDeviceCaptureLinuxFailedToCreateVideoCaptureDelegate:
      return media::mojom::VideoCaptureError::
          kDeviceCaptureLinuxFailedToCreateVideoCaptureDelegate;
    case media::VideoCaptureError::
        kErrorFakeDeviceIntentionallyEmittingErrorEvent:
      return media::mojom::VideoCaptureError::
          kErrorFakeDeviceIntentionallyEmittingErrorEvent;
    case media::VideoCaptureError::kDeviceClientTooManyFramesDroppedY16:
      return media::mojom::VideoCaptureError::
          kDeviceClientTooManyFramesDroppedY16;
    case media::VideoCaptureError::
        kDeviceMediaToMojoAdapterEncounteredUnsupportedBufferType:
      return media::mojom::VideoCaptureError::
          kDeviceMediaToMojoAdapterEncounteredUnsupportedBufferType;
    case media::VideoCaptureError::
        kVideoCaptureManagerProcessDeviceStartQueueDeviceInfoNotFound:
      return media::mojom::VideoCaptureError::
          kVideoCaptureManagerProcessDeviceStartQueueDeviceInfoNotFound;
    case media::VideoCaptureError::
        kInProcessDeviceLauncherFailedToCreateDeviceInstance:
      return media::mojom::VideoCaptureError::
          kInProcessDeviceLauncherFailedToCreateDeviceInstance;
    case media::VideoCaptureError::
        kServiceDeviceLauncherLostConnectionToDeviceFactoryDuringDeviceStart:
      return media::mojom::VideoCaptureError::
          kServiceDeviceLauncherLostConnectionToDeviceFactoryDuringDeviceStart;
    case media::VideoCaptureError::
        kServiceDeviceLauncherServiceRespondedWithDeviceNotFound:
      return media::mojom::VideoCaptureError::
          kServiceDeviceLauncherServiceRespondedWithDeviceNotFound;
    case media::VideoCaptureError::
        kServiceDeviceLauncherConnectionLostWhileWaitingForCallback:
      return media::mojom::VideoCaptureError::
          kServiceDeviceLauncherConnectionLostWhileWaitingForCallback;
    case media::VideoCaptureError::kIntentionalErrorRaisedByUnitTest:
      return media::mojom::VideoCaptureError::kIntentionalErrorRaisedByUnitTest;
    case media::VideoCaptureError::kCrosHalV3FailedToStartDeviceThread:
      return media::mojom::VideoCaptureError::
          kCrosHalV3FailedToStartDeviceThread;
    case media::VideoCaptureError::kCrosHalV3DeviceDelegateMojoConnectionError:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateMojoConnectionError;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToGetCameraInfo:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToGetCameraInfo;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateMissingSensorOrientationInfo:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateMissingSensorOrientationInfo;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToOpenCameraDevice:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToOpenCameraDevice;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToInitializeCameraDevice:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToInitializeCameraDevice;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToConfigureStreams:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToConfigureStreams;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateWrongNumberOfStreamsConfigured:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateWrongNumberOfStreamsConfigured;
    case media::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToGetDefaultRequestSettings:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToGetDefaultRequestSettings;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerHalRequestedTooManyBuffers:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerHalRequestedTooManyBuffers;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToCreateGpuMemoryBuffer:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToCreateGpuMemoryBuffer;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToMapGpuMemoryBuffer:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToMapGpuMemoryBuffer;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerUnsupportedVideoPixelFormat:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerUnsupportedVideoPixelFormat;
    case media::VideoCaptureError::kCrosHalV3BufferManagerFailedToDupFd:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToDupFd;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToWrapGpuMemoryHandle:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToWrapGpuMemoryHandle;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToRegisterBuffer:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToRegisterBuffer;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerProcessCaptureRequestFailed:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerProcessCaptureRequestFailed;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerInvalidPendingResultId:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerInvalidPendingResultId;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedDuplicatedPartialMetadata:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedDuplicatedPartialMetadata;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerIncorrectNumberOfOutputBuffersReceived:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerIncorrectNumberOfOutputBuffersReceived;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerInvalidTypeOfOutputBuffersReceived:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerInvalidTypeOfOutputBuffersReceived;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedMultipleResultBuffersForFrame:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedMultipleResultBuffersForFrame;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerUnknownStreamInCamera3NotifyMsg:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerUnknownStreamInCamera3NotifyMsg;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedInvalidShutterTime:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedInvalidShutterTime;
    case media::VideoCaptureError::kCrosHalV3BufferManagerFatalDeviceError:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFatalDeviceError;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedFrameIsOutOfOrder:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedFrameIsOutOfOrder;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToUnwrapReleaseFenceFd:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToUnwrapReleaseFenceFd;
    case media::VideoCaptureError::
        kCrosHalV3BufferManagerSyncWaitOnReleaseFenceTimedOut:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerSyncWaitOnReleaseFenceTimedOut;
    case media::VideoCaptureError::kCrosHalV3BufferManagerInvalidJpegBlob:
      return media::mojom::VideoCaptureError::
          kCrosHalV3BufferManagerInvalidJpegBlob;
    case media::VideoCaptureError::kAndroidFailedToAllocate:
      return media::mojom::VideoCaptureError::kAndroidFailedToAllocate;
    case media::VideoCaptureError::kAndroidFailedToStartCapture:
      return media::mojom::VideoCaptureError::kAndroidFailedToStartCapture;
    case media::VideoCaptureError::kAndroidFailedToStopCapture:
      return media::mojom::VideoCaptureError::kAndroidFailedToStopCapture;
    case media::VideoCaptureError::kAndroidApi1CameraErrorCallbackReceived:
      return media::mojom::VideoCaptureError::
          kAndroidApi1CameraErrorCallbackReceived;
    case media::VideoCaptureError::kAndroidApi2CameraDeviceErrorReceived:
      return media::mojom::VideoCaptureError::
          kAndroidApi2CameraDeviceErrorReceived;
    case media::VideoCaptureError::kAndroidApi2CaptureSessionConfigureFailed:
      return media::mojom::VideoCaptureError::
          kAndroidApi2CaptureSessionConfigureFailed;
    case media::VideoCaptureError::kAndroidApi2ImageReaderUnexpectedImageFormat:
      return media::mojom::VideoCaptureError::
          kAndroidApi2ImageReaderUnexpectedImageFormat;
    case media::VideoCaptureError::
        kAndroidApi2ImageReaderSizeDidNotMatchImageSize:
      return media::mojom::VideoCaptureError::
          kAndroidApi2ImageReaderSizeDidNotMatchImageSize;
    case media::VideoCaptureError::kAndroidApi2ErrorRestartingPreview:
      return media::mojom::VideoCaptureError::
          kAndroidApi2ErrorRestartingPreview;
    case media::VideoCaptureError::kAndroidScreenCaptureUnsupportedFormat:
      return media::mojom::VideoCaptureError::
          kAndroidScreenCaptureUnsupportedFormat;
    case media::VideoCaptureError::
        kAndroidScreenCaptureFailedToStartCaptureMachine:
      return media::mojom::VideoCaptureError::
          kAndroidScreenCaptureFailedToStartCaptureMachine;
    case media::VideoCaptureError::
        kAndroidScreenCaptureTheUserDeniedScreenCapture:
      return media::mojom::VideoCaptureError::
          kAndroidScreenCaptureTheUserDeniedScreenCapture;
    case media::VideoCaptureError::
        kAndroidScreenCaptureFailedToStartScreenCapture:
      return media::mojom::VideoCaptureError::
          kAndroidScreenCaptureFailedToStartScreenCapture;
    case media::VideoCaptureError::kWinDirectShowCantGetCaptureFormatSettings:
      return media::mojom::VideoCaptureError::
          kWinDirectShowCantGetCaptureFormatSettings;
    case media::VideoCaptureError::
        kWinDirectShowFailedToGetNumberOfCapabilities:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToGetNumberOfCapabilities;
    case media::VideoCaptureError::
        kWinDirectShowFailedToGetCaptureDeviceCapabilities:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToGetCaptureDeviceCapabilities;
    case media::VideoCaptureError::
        kWinDirectShowFailedToSetCaptureDeviceOutputFormat:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToSetCaptureDeviceOutputFormat;
    case media::VideoCaptureError::kWinDirectShowFailedToConnectTheCaptureGraph:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToConnectTheCaptureGraph;
    case media::VideoCaptureError::kWinDirectShowFailedToPauseTheCaptureDevice:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToPauseTheCaptureDevice;
    case media::VideoCaptureError::kWinDirectShowFailedToStartTheCaptureDevice:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToStartTheCaptureDevice;
    case media::VideoCaptureError::kWinDirectShowFailedToStopTheCaptureGraph:
      return media::mojom::VideoCaptureError::
          kWinDirectShowFailedToStopTheCaptureGraph;
    case media::VideoCaptureError::kWinMediaFoundationEngineIsNull:
      return media::mojom::VideoCaptureError::kWinMediaFoundationEngineIsNull;
    case media::VideoCaptureError::kWinMediaFoundationEngineGetSourceFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationEngineGetSourceFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationFillPhotoCapabilitiesFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationFillPhotoCapabilitiesFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationFillVideoCapabilitiesFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationFillVideoCapabilitiesFailed;
    case media::VideoCaptureError::kWinMediaFoundationNoVideoCapabilityFound:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationNoVideoCapabilityFound;
    case media::VideoCaptureError::
        kWinMediaFoundationGetAvailableDeviceMediaTypeFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationGetAvailableDeviceMediaTypeFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationSetCurrentDeviceMediaTypeFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationSetCurrentDeviceMediaTypeFailed;
    case media::VideoCaptureError::kWinMediaFoundationEngineGetSinkFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationEngineGetSinkFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationSinkQueryCapturePreviewInterfaceFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationSinkQueryCapturePreviewInterfaceFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationSinkRemoveAllStreamsFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationSinkRemoveAllStreamsFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationCreateSinkVideoMediaTypeFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationCreateSinkVideoMediaTypeFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationConvertToVideoSinkMediaTypeFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationConvertToVideoSinkMediaTypeFailed;
    case media::VideoCaptureError::kWinMediaFoundationSinkAddStreamFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationSinkAddStreamFailed;
    case media::VideoCaptureError::
        kWinMediaFoundationSinkSetSampleCallbackFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationSinkSetSampleCallbackFailed;
    case media::VideoCaptureError::kWinMediaFoundationEngineStartPreviewFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationEngineStartPreviewFailed;
    case media::VideoCaptureError::kWinMediaFoundationGetMediaEventStatusFailed:
      return media::mojom::VideoCaptureError::
          kWinMediaFoundationGetMediaEventStatusFailed;
    case media::VideoCaptureError::kMacSetCaptureDeviceFailed:
      return media::mojom::VideoCaptureError::kMacSetCaptureDeviceFailed;
    case media::VideoCaptureError::kMacCouldNotStartCaptureDevice:
      return media::mojom::VideoCaptureError::kMacCouldNotStartCaptureDevice;
    case media::VideoCaptureError::kMacReceivedFrameWithUnexpectedResolution:
      return media::mojom::VideoCaptureError::
          kMacReceivedFrameWithUnexpectedResolution;
    case media::VideoCaptureError::kMacUpdateCaptureResolutionFailed:
      return media::mojom::VideoCaptureError::kMacUpdateCaptureResolutionFailed;
    case media::VideoCaptureError::kMacDeckLinkDeviceIdNotFoundInTheSystem:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkDeviceIdNotFoundInTheSystem;
    case media::VideoCaptureError::kMacDeckLinkErrorQueryingInputInterface:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkErrorQueryingInputInterface;
    case media::VideoCaptureError::kMacDeckLinkErrorCreatingDisplayModeIterator:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkErrorCreatingDisplayModeIterator;
    case media::VideoCaptureError::kMacDeckLinkCouldNotFindADisplayMode:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkCouldNotFindADisplayMode;
    case media::VideoCaptureError::
        kMacDeckLinkCouldNotSelectTheVideoFormatWeLike:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkCouldNotSelectTheVideoFormatWeLike;
    case media::VideoCaptureError::kMacDeckLinkCouldNotStartCapturing:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkCouldNotStartCapturing;
    case media::VideoCaptureError::kMacDeckLinkUnsupportedPixelFormat:
      return media::mojom::VideoCaptureError::
          kMacDeckLinkUnsupportedPixelFormat;
    case media::VideoCaptureError::
        kMacAvFoundationReceivedAVCaptureSessionRuntimeErrorNotification:
      return media::mojom::VideoCaptureError::
          kMacAvFoundationReceivedAVCaptureSessionRuntimeErrorNotification;
     case media::VideoCaptureError::kAndroidApi2ErrorConfiguringCamera:
       return media::mojom::VideoCaptureError::
           kAndroidApi2ErrorConfiguringCamera;
    case media::VideoCaptureError::kCrosHalV3DeviceDelegateFailedToFlush:
      return media::mojom::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToFlush;
   }
   NOTREACHED();
   return media::mojom::VideoCaptureError::kNone;
}
