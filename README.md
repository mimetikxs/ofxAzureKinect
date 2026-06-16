# ofxAzureKinect

ofxAddon that allows you to use [Azure Kinect](https://azure.microsoft.com/en-us/services/kinect-dk/) in [openFrameworks](https://github.com/openframeworks/openFrameworks).

* Get depth, color, depth to world, and color in depth frames as `ofPixels` or `ofTexture`.
* Get point cloud VBO with texture coordinates in depth space.
* Get body tracking skeleton and index texture.
* Use multiple sensors per machine (tested up to 4!)
* Set up sync mode (standalone, master, subordinate) with multiple devices when connected with sync cables.
* Record and playback streams.
* More coming soon...

## Installation

The instructions below are based on the [Azure Kinect Sensor SDK Usage](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/docs/usage.md) page.

### Backend 1: Microsoft Azure Kinect

#### Windows

* Install the [Azure Kinect Sensor SDK](https://docs.microsoft.com/en-us/azure/Kinect-dk/sensor-sdk-download).
* Install the [Azure Kinect Body Tracking SDK](https://docs.microsoft.com/en-us/azure/Kinect-dk/body-sdk-download).
* Add an environment variable for `AZUREKINECT_SDK` and set it to the Sensor SDK installation path (no trailing slash). The default is `C:\Program Files\Azure Kinect SDK v1.4.1`.
* Add an environment variable for `AZUREKINECT_BODY_SDK` and set it to the Body SDK installation path (no trailing slash). The default is `C:\Program Files\Azure Kinect Body Tracking SDK`.

 ![Environment Variables](Install-EnvVars.png)

* Add the path to the Sensor SDK `bin` folder to the `PATH` variable. The default is `%AZUREKINECT_SDK%\sdk\windows-desktop\amd64\release\bin`.
* Add the path to the Body SDK `bin` folder to the `PATH` variable. The default is `%AZUREKINECT_BODY_SDK%\sdk\windows-desktop\amd64\release\bin`.
* Add the path to the Body SDK `tools` folder to the `PATH` variable. The default is `%AZUREKINECT_BODY_SDK%\tools`.

 ![Path](Install-Path.png)

* Clone this repository in your openFrameworks `addons` folder.
* You can then use the OF Project Generator to generate projects with the appropriate headers and libraries included. ✌️
* Note that if you want to use body tracking, you will need to copy the cuDNN model file `dnn_model_2_0.onnx` from the Body SDK `tools` folder into your project's `bin` folder!

#### Linux

*(Note: These installation instructions have been tested and verified on Ubuntu 24.04)*

* Configure the [Linux Software Repository for Microsoft](https://docs.microsoft.com/en-us/windows-server/administration/linux-package-repository-for-microsoft-software). Note that for Ubuntu you'll need to set up the repo for 18.04 even if you're running newer versions.
* Install the Azure Kinect Sensor SDK packages. It is important to install the specific versions in this order:

  ```bash
  sudo apt install libk4a1.4=1.4.1 libk4a1.4-dev=1.4.1 k4a-tools=1.4.1
  ```

* Install the Azure Kinect Body Tracking SDK packages. The version defaults to `1.1.2`:

  ```bash
  sudo apt install libk4abt1.1 libk4abt1.1-dev
  ```

* Setup udev rules by copying [this file](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/scripts/99-k4a.rules) to `/etc/udev/rules.d/99-k4a.rules`.
* Install [libjpeg-turbo](https://sourceforge.net/projects/libjpeg-turbo/).
* Clone this repository in your openFrameworks `addons` folder.
* You can then use the OF Project Generator to generate projects with the appropriate headers and libraries included.
🐣

More info about installing dependencies: [Azure Kinect on Ubuntu](https://gist.github.com/mimetikxs/a89cd261481d092784e0cbe02bbd2b27).

### Backend 2: Orbbec Femto Bolt

To use the Orbbec Femto Bolt, this addon natively supports compiling against the Orbbec K4A Wrapper as a drop-in replacement.

#### Windows

1. Ensure the standard Microsoft `Azure Kinect Body Tracking SDK` is installed if you intend to use body tracking features *(Note: you already have this if you set up the Microsoft backend previously)*.
2. Download the latest Windows (`win_x64`) release from the [OrbbecSDK-K4A-Wrapper Releases](https://github.com/orbbec/OrbbecSDK-K4A-Wrapper/releases). **Important**: Ensure you download a release from the `v2-main` branch (e.g., `v2.x.x`) to use the latest Orbbec SDK v2.
3. Extract the downloaded archive to a dedicated folder (e.g., `C:\OrbbecSDK-K4A-Wrapper`).
4. Add an environment variable for `ORBBEC_K4A_WRAPPER` and set it to this extracted folder path.
5. Add the path to the wrapper's `bin` folder (e.g., `%ORBBEC_K4A_WRAPPER%\bin`) to your system `PATH` variable so the depth engine and `k4a.dll` can be found at runtime.
6. Open `addons/ofxAzureKinect/addon_config.mk` and follow the instructions in the `vs:` section to comment out the default Microsoft backend and uncomment the Orbbec backend.
7. Use the OF Project Generator to update your project.

#### Linux

1. Ensure the standard Microsoft `libk4abt` package is installed on your system if you intend to use body tracking features *(Note: you already have this if you set up the Microsoft backend previously)*.
2. Download the latest Linux release from the [OrbbecSDK-K4A-Wrapper Releases](https://github.com/orbbec/OrbbecSDK-K4A-Wrapper/releases). **Important**: Ensure you download a release from the `v2-main` branch (e.g., `v2.x.x`) to use the latest Orbbec SDK v2.
3. Extract the downloaded archive to a dedicated folder: `/opt/orbbec-k4a/`.
4. Set up the Orbbec `udev` rules. Since you downloaded the v2 wrapper, it provides its own uniquely named rules file that won't conflict with Microsoft's:
   ```bash
   sudo cp /opt/orbbec-k4a/scripts/99-obsensor-libusb.rules /etc/udev/rules.d/
   sudo udevadm control --reload-rules && sudo udevadm trigger
   ```
   *(Alternatively, you can just run `sudo ./install_udev_rules.sh` inside that scripts folder).*
5. Open `addons/ofxAzureKinect/addon_config.mk` and follow the instructions in the `linux64:` section to comment out the default Microsoft backend and uncomment the Orbbec backend.
6. Clean and recompile your project!

## Compatibility

Tested with:

* openFrameworks 0.12.1
* Windows 10/11, Visual Studio 2019/2020
* Ubuntu 24.04 LTS, VS Code

## Examples

Use the OF Project Generator to create the example project files. If everything is installed correctly, it should properly locate all required include and library files.

* `example-streams` demonstrates how to get depth, color, infrared textures from the device.
* `example-scaled-depth` demonstrates how to remap the depth data to a narrower (probably more useful) range.
* `example-pointCloud` demonstrates how to draw the basic point cloud VBO from the device.
* `example-world-coord` demonstrates how to get the world coordinate from the depth and depth to world data sets.
* `example-shader` demonstrates how to reconstruct a point cloud using LUTs in a shader.
* `example-bodies` demonstrates how to get the body tracking index texture and skeleton joint information in 3D.
* `example-bodies-projected` demonstrates how to get the body tracking index texture and skeleton joint information in 2D.
* `example-multi` demonstrates how to use multiple devices in a single app.
* `example-record` demonstrates how to record and playback device streams.
