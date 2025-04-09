if [ -d "cmake-build-debug" ]; then
    echo "Directory exists."
else
    mkdir cmake-build-debug
fi

cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .