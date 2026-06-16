# NVIDIA Optimus (Hybrid Graphics) on Linux: Error 204

When running `ofxAzureKinect` applications on Linux laptops with dual GPUs (Intel/AMD integrated + NVIDIA Dedicated) using **PRIME Render Offload** (Hybrid Graphics), the application may crash and produce the following error trace:

```text
GlContext::Initialize eglMakeCurrent error code = 0x3000
[error] transform_engine_start_helper(). Transform engine create and initialize failed with error code: 204.
[error] transform_engine_start_helper(tewrapper) returned failure in transform_engine_thread()
[error] k4a_transformation_t_get_context(). Invalid k4a_transformation_t (nil)
```

## The Cause

This is a known, hard limitation of the Orbbec SDK and the proprietary NVIDIA Linux drivers.

The Depth Engine relies on an OpenGL/EGL compute shader pipeline to convert raw phase data into depth pixels. To achieve 30fps performance, the Depth Engine attempts to allocate "zero-copy" memory buffers (DMA-BUFs) that are shared directly between the CPU and the GPU via the `EGL_EXT_image_dma_buf_import` extension.

When using PRIME Render Offload (e.g., launching with `__NV_PRIME_RENDER_OFFLOAD=1`), the NVIDIA GPU handles rendering while the Intel/AMD GPU manages the display buffers. The proprietary NVIDIA driver frequently fails to negotiate these cross-GPU EGL DMA-BUF imports for closed-source background threads. Consequently, the closed-source `libdepthengine.so` binary blob crashes attempting to initialize its EGL context, resulting in `Error 204`.

This cannot be bypassed with C++ code or environment variable hacks (like `unsetenv` or `__EGL_VENDOR_LIBRARY_FILENAMES`), as the `libglvnd` dispatcher resolves the drivers globally at process startup before the application code executes.

## The Workaround: NVIDIA-Only Mode

To utilize your Dedicated NVIDIA GPU for high-performance openFrameworks rendering alongside the depth sensor, you must disable Hybrid/Optimus mode and configure your laptop to run entirely on the NVIDIA GPU (Discrete Mode).

When the NVIDIA GPU acts as the primary and only display controller, the Intel/AMD GPU is bypassed, eliminating the need for complex cross-GPU DMA-BUF negotiations, allowing the Depth Engine to initialize flawlessly.

### Ubuntu / Debian-based distributions
Open a terminal and run the `prime-select` utility to switch to the NVIDIA profile:

```bash
sudo prime-select nvidia
```

*You must restart your computer for this change to take effect.*

#### Restoring Hybrid Mode
If you need to restore your laptop to Hybrid mode later (to save battery life when not running the camera), simply open a terminal and run:

```bash
sudo prime-select on-demand
```
*(Or `sudo prime-select intel` to disable the NVIDIA GPU completely). Reboot your laptop for the changes to take effect.*

### BIOS / UEFI Switch (MUX Switch)
If your laptop has a hardware MUX switch, reboot your machine, enter the BIOS/UEFI settings, and change the graphics mode from **"Hybrid" / "Optimus" / "Dynamic"** to **"Discrete" / "Dedicated" / "dGPU Only"**.

Once running in Discrete mode, you no longer need to prefix your application with `__NV_PRIME_RENDER_OFFLOAD=1`. You can launch your compiled binary natively, and both openFrameworks and the Depth Engine will utilize the NVIDIA GPU.

## Implications of NVIDIA-Only (Discrete) Mode

Switching your laptop from Hybrid mode to Discrete (NVIDIA-Only) mode has a few practical implications for your daily usage:

1. **Battery Life**: The NVIDIA GPU will be permanently powered on and driving your laptop display. This will significantly increase power consumption and reduce your laptop's battery life when unplugged.
2. **Heat and Fan Noise**: Because the Dedicated GPU is constantly active, your laptop will generate more heat and the cooling fans will likely run more frequently, even during basic web browsing.
3. **Maximum Performance**: On the positive side, running in Discrete mode eliminates the latency penalty of copying display buffers from the NVIDIA GPU to the Intel GPU. This often results in a slight performance boost and reduced input lag for all graphics-intensive applications (including openFrameworks and games).

If you use your laptop on battery frequently, you will likely want to use `sudo prime-select on-demand` to restore Hybrid mode when you are not actively working with the depth camera.

## Further Reading & Upstream Bug Reports

The core issue lies within `libdepthengine.so` (or Orbbec's `libDepthengine_2_0.so`). Because the Orbbec Wrapper is built on the original Microsoft Azure Kinect architecture, it inherited the exact same closed-source depth engine binary blob. 

This binary rigidly relies on legacy OpenGL/EGL initialization paths that completely break down in headless environments or across Optimus DMA-BUF boundaries. This is a notoriously well-documented issue across the community:

*   **[Azure Kinect Issue #1093](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/1093)**: Depth Engine crashes on Optimus laptops.
*   **[Azure Kinect Issue #1311](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/1311)**: Error 204 specifically tied to Linux NVIDIA Prime Render Offload setups.
*   **[Azure Kinect Issue #1449](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/1449)**: Error 204 occurring over SSH or in Headless Server environments due to the strict OpenGL/X11 dependency.
