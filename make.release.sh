if [ -d "cmake-build-release" ]; then
    echo "Directory exists."
else
    mkdir cmake-build-release
fi

cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .