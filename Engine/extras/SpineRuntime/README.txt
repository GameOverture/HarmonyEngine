Please see the Spine Runtimes License before integrating the Spine Runtimes into your applications.
http://en.esotericsoftware.com/spine-runtimes-license

1. Clone the spine-runtimes Git repository. If you don't want to use Git, download the latest version as a ZIP and unpack it.
   https://github.com/esotericsoftware/spine-runtimes - Repo
   http://esotericsoftware.com/git/spine-runtimes/archive - ZIP
   
2. Copy the folder 'spine-runtimes/spine-cpp' to 'HarmonyEngine/Engine/extras/SpineRuntime' aka this README.txt location

3. Create new builds that enable the Spine Runtime
   CMake option: -DHYBUILD_SPINE=ON