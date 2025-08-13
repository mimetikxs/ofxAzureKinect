#pragma once

#include <string>

#include <k4a/k4a.hpp>
#include <k4abt.h>

#include "ofPixels.h"
#include "ofTexture.h"

#include "Recorder.h"
#include "Stream.h"
#include "Types.h"

namespace ofxAzureKinect
{
	struct DeviceSettings
	{
		DepthMode depthMode;
		ColorResolution colorResolution;
		ImageFormat colorFormat;
		FramesPerSecond cameraFps;

		WiredSyncMode wiredSyncMode;
		uint32_t depthDelayUsec;
		uint32_t subordinateDelayUsec;

		bool updateColor;
		bool updateIr;
		bool updateWorld;
		bool updateVbo;
		bool forceVboToDepthSize;

		bool syncImages;

		DeviceSettings();
	};

	class Device 
		: public Stream
	{
	public:
		static int getInstalledCount();

	public:
		Device();
		~Device();

		bool open(uint32_t idx = 0);
		bool open(const std::string& serialNumber);
		bool close();

		bool startCameras(DeviceSettings deviceSettings = DeviceSettings());
		bool stopCameras();

		bool startRecording(std::string filepath = "");
		bool stopRecording();

		bool isSyncInConnected() const;
		bool isSyncOutConnected() const;

		bool isRecording() const;

		DepthMode getDepthMode() const override;
		ImageFormat getColorFormat() const override;
		ColorResolution getColorResolution() const override;
		FramesPerSecond getCameraFps() const override;

		WiredSyncMode getWiredSyncMode() const override;
		uint32_t getDepthDelayUsec() const override;
		uint32_t getSubordinateDelayUsec() const override;

		bool getSyncImages() const;

		const Recorder& getRecorder() const;
		Recorder& getRecorder();

		// Color camera controls
		// there are some issues with exposure: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/1511
		struct ControlInfo
		{
			k4a_color_control_mode_t mode; 
			int32_t value;
		};
		void setColorControl(k4a_color_control_command_t command, k4a_color_control_mode_t mode, int32_t value);
		ControlInfo getColorControl(k4a_color_control_command_t command);

	protected:
		bool updateCapture() override;

		void updatePixels() override;

	private:
		int index;
	
		bool bRecording;

		k4a_device_configuration_t config;
		k4a::device device;
		
		Recorder recorder;
	};
}
