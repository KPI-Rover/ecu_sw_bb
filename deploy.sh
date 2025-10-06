#!/bin/bash

# Check if BBB_HOST is set
if [ -z "$BBB_HOST" ]; then
    # Try to use host from ~/.ssh/config named kpi-rover-bbb
    if ssh -G kpi-rover-bbb &>/dev/null; then
        BBB_HOST="kpi-rover-bbb"
        echo "Info: BBB_HOST not set, using host from ~/.ssh/config: $BBB_HOST"
    else
        echo "Error: BBB_HOST environment variable is not set and no 'kpi-rover-bbb' host found in ~/.ssh/config."
        echo "Hint: Set it using 'export BBB_HOST=debian@<BeagleBone Blue IP>' or define 'kpi-rover-bbb' in your ~/.ssh/config."
        exit 1
    fi
fi

# Deploy the software using scp
BBB_HOST_ROOT="${BBB_HOST/debian/root}"

echo "Copying the binary to the BeagleBone Blue"
scp build/Target/src/kpi_rover_ecu/kpi_rover_ecu "$BBB_HOST":~

# echo "Copying the systemd service file to the BeagleBone Blue"
# scp system/kpi_rover_ecu.service "$BBB_HOST_ROOT":/etc/systemd/system

# echo "Reloading systemd daemon on $BBB_HOST"
# ssh -t "$BBB_HOST" 'sudo systemctl daemon-reload'

# # Check if scp was successful
# if [ $? -eq 0 ]; then
#     echo "Deployment to $BBB_HOST completed successfully."
# else
#     echo "Deployment failed. Check your connection and try again."
#     exit 1
# fi
