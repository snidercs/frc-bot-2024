# Bot 2024

## Features
- Built-in Lua interpreter.
- Smart bot configuration with Lua bindings and negligible overhead.
- Fast code deploys when c++ doesn't need recompiled.
- Ultra low latency performance.
- Design is decoupled from wpilib making it portable to other robotics systems.

## VScode Dommands
There are shortcuts to command line tasks.

Press `shift + ctrl + p` to open a list of commands. Type `wpilib` to filter FRC specific actions.

## Requirements

I recommend at least these two tools:
- [VSCode](https://code.visualstudio.com/)
- [Docker](https://www.docker.com/products/docker-desktop)
    - Docker is used to build the code in a consistent environment. It is not required, but without it you'll have to install java, gradle, a comiler, and other tools on your computer, and good luck keeping it all up to date. 
    - Make sure you give at least 4GB RAM and 4GB of swap space to docker.  Gradle build is a memory hog.

### Optional Windows tools
- wsl2 (Windows Subsystem for Linux)
- [Git](https://git-scm.com/downloads)
- consider choco.  It's the best package manager for windows.

### Optional Mac Tools
- use [brew]().  It's the best package manager for mac.

## Building

If it's your first time, it takes a while.  So get some lunch for the first time.   On Mac M2:
```
BUILD SUCCESSFUL in 36m 10s
18 actionable tasks: 18 executed
```


Build all targets, and run the simulator.
```bash
./gradlew build
```

Using docker:
```bash
docker run -it -v $(pwd):/root  wpilib/roborio-cross-ubuntu:2024-22.04
cd /root
./gradlew build 
```

Once, built:
```bash
BUILD SUCCESSFUL in 19s
18 actionable tasks: 18 up-to-date
```

## Testing
Run all unit tests.
```bash
./gradlew check

## Deployment
Run the following command to deploy code to the roboRIO
```bash
./gradlew deploy
```

If it gives problems, cleaning the project could help. The `--info` option could give more information too.
```bash
./gradlew clean
./gradlew deploy --info

```

## Other Useful Terminal Commands
Useful Commands to Understand
```bash
git fetch        # Fetch changes from github (needed to check out new ones)
git status -a    # Shows the status of all git branches
git checkout .   # Resets current changes on the branch
git checkout new_branch_name    # Switches branches to new_branch_name from a different branch
git pull         # Pulls new code onto the branch
```

## Simulation
Bot simulation is available in a few ways...

**IMPORTANT**: The `Map gamepad` in the controllers list must be checked to mock the real bot.

### Basic Simulator
This is the data-based simulator panel.

```bash
# run this from the command line to start the basic simulator
./gradlew simulateNative
```

![](doc/images/basic-simulator.png)

### Extern Sim With Real Driver Station

```bash
./gradlew simulateExternalNativeDebug
```
or
```bash
./gradlew simulateExternalNativeRelease
```

## Feedforward and Feedback Control

[Picking a Control Mechanism](https://docs.wpilib.org/en/stable/docs/software/advanced-controls/introduction/picking-control-strategy.html)


## Driver Station Tuning & Deployment
* [Flashing the OS](https://docs.wpilib.org/en/stable/docs/zero-to-robot/step-3/imaging-your-roborio.html)
* [Deployment Info](https://docs.wpilib.org/en/stable/docs/software/vscode-overview/deploying-robot-code.html#building-and-deploying-robot-code)
* [Driver Station Best Practices](https://frcdocs.wpi.edu/en/2022/docs/software/driverstation/driver-station-best-practices.html)

## Camera Example

When CameraServer opens a camera, it creates a webpage that you can use to view the camera stream and view the effects of various camera settings. To connect to the web interface, use a web browser to navigate to [http://roboRIO-TEAM-frc.local:1181](http://roboRIO-TEAM-frc.local:1181). There is no additional code needed other then Simple CameraServer Program.  If running in the *simulator* navigate to [http://localhost:1181](http://localhost:1181)

* [Example Code](https://docs.wpilib.org/en/stable/docs/software/vision-processing/roborio/using-the-cameraserver-on-the-roborio.html)

## Example Code

FRC provides [several examples](https://github.com/wpilibsuite/allwpilib/tree/main/wpilibcExamples/src/main/cpp/examples) of how to utilize WPIlib on GitHub.  It is easier, and faster to copy and paste from these rather than generate new projects all the time.

* [All cpp Examples](https://github.com/wpilibsuite/allwpilib/tree/main/wpilibcExamples/src/main/cpp/examples)

## Differential Drive Help

* [PWM Controllers](https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/pwm-controllers.html)
* [Drive classes](https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html)
