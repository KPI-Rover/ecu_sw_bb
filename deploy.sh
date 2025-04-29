#!/bin/bash

# Check if BBB_HOST is set
if [ -z "$BBB_HOST" ]; then
    echo "Error: BBB_HOST environment variable is not set."
    echo "Hint: Set it using 'export BBB_HOST=debian@<BeagleBone Blue IP>'"
    exit 1
fi

# Deploy the software using scp
BBB_HOST_ROOT="${BBB_HOST/debian/root}"

scp build/Target/src/kpi_rover_ecu/kpi_rover_ecu "$BBB_HOST":~
scp system/kpi_rover_ecu.service "$BBB_HOST_ROOT":/etc/systemd/system
ssh "$BBB_HOST_ROOT" 'systemctl daemon-reload'

# Check if scp was successful
if [ $? -eq 0 ]; then
    echo "Deployment to $BBB_HOST completed successfully."
else
    echo "Deployment failed. Check your connection and try again."
    exit 1
fi
