#!/bin/bash

docker run -it --rm \
    -u $(id -u ${USER}):$(id -g ${USER}) \
    -v $(pwd):/workspace \
    kpi-rover-bbb-check \
    bash -c "
        cmake -DBUILD_VARIANT=Target -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build/check && ls build/check/compile_commands.json && \
        find src/kpi_rover_ecu/src src/kpi_rover_ecu/include -name '*.cpp' -o -name '*.h' | xargs --no-run-if-empty clang-format --dry-run --Werror; \
        find src/kpi_rover_ecu/src src/kpi_rover_ecu/include -name '*.cpp' -o -name '*.h' | xargs --no-run-if-empty clang-tidy -p=build/check --warnings-as-errors='*'
    "

