IF NOT EXIST build_vis (
  mkdir build_vis
)

cd build_vis
cmake .. -DCMAKE_INSTALL_PREFIX=../install_vis -G "Visual Studio 12" -T "v120_xp" -DBUILD_CEF=OFF -DDEBUG=ON -DBUILD_TESTS=ON -DBUILD_TESTING=ON -DDEBUG_V8=ON
start Desura.sln
cd ..