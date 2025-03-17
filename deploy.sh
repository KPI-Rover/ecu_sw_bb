#!/bin/bash

# Check if BBB_HOST is set
if [ -z "$BBB_HOST" ]; then
    echo "Error: BBB_HOST environment variable is not set."
    echo "Hint: Set it using 'export BBB_HOST=debian@<BeagleBone Blue IP>'"
    exit 1
fi

# Deploy the software using scp
scp build/Target/src/kpi_rover_ecu/kpi_rover_ecu "$BBB_HOST":~

# Check if scp was successful
if [ $? -eq 0 ]; then
    echo "Deployment to $BBB_HOST completed successfully."
else
    echo "Deployment failed. Check your connection and try again."
    exit 1
fi
