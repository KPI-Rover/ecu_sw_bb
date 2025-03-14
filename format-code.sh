#!/bin/bash

# Script to format C/C++ code using clang-format within the kpi-rover Docker container

# Run Docker container with current user permissions
docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-check \
    bash -c "echo 'Finding files to format in src directory...' && \
             [ -d ./src ] || { echo 'Error: src directory not found!'; exit 1; } && \
             FILES=\$(find ./src -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.hpp') && \
             echo \"Found \$(echo \$FILES | wc -w) files to format\" && \
             echo 'Starting formatting...' && \
             clang-format -i -verbose \$FILES && \
             echo 'Formatting complete'"

echo "Code formatting complete"
