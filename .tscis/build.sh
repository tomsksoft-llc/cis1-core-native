mkdir build
cd build

conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. --profile $cis_base_dir/jobs/$job_name/../conan_profile -s build_type=Release --build=missing

cmake $cis_base_dir/jobs/$job_name/shared_srcs -DBUILD_TESTING=True -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-static-libstdc++"

cmake --build .

./test_package/bin/tests

if [ $? -eq 0 ]
then
  echo "Tests finished successfully"
else
  echo "Tests failed"
  exit 1
fi

cp bin/* $cis_base_dir/jobs/$job_name/$build_number/artifacts
