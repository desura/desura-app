IF NOT EXIST build_vis (
  mkdir build_vis
)

cd build_vis
cmake .. -DCMAKE_INSTALL_PREFIX=../install_vis -G "Visual Studio 12" -T "v120_xp" -DBUILD_CEF=OFF -DDEBUG=OFF -DBUILD_TESTS=ON -DBUILD_TESTING=ON -DDEBUG_V8=OFF
start Desura.sln
cd ..