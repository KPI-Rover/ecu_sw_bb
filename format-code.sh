#!/bin/bash

# Script to format C/C++ code using clang-format within the kpi-rover Docker container

# Run Docker container with current user permissions
docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-check \
    bash -c "find . -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp' | xargs clang-format -i"

echo "Code formatting complete"
