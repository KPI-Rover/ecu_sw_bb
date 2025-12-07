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

# Set remote and local log paths
REMOTE_LOG_DIR="~/log"
LOCAL_LOG_DIR="./logs"

# Clean local log directory
if [ -d "$LOCAL_LOG_DIR" ]; then
    echo "Cleaning local log directory..."
    rm -rf "$LOCAL_LOG_DIR"/*
fi

# Create local log directory if it doesn't exist
mkdir -p "$LOCAL_LOG_DIR"

echo "Fetching log files from BeagleBone Blue..."

# Get the actual filenames that the symlinks point to and copy them
INFO_FILE=$(ssh "$BBB_HOST" "readlink -f $REMOTE_LOG_DIR/kpi_rover_ecu.INFO 2>/dev/null")
WARNING_FILE=$(ssh "$BBB_HOST" "readlink -f $REMOTE_LOG_DIR/kpi_rover_ecu.WARNING 2>/dev/null")
ERROR_FILE=$(ssh "$BBB_HOST" "readlink -f $REMOTE_LOG_DIR/kpi_rover_ecu.ERROR 2>/dev/null")

# Copy files if they exist
if [ -n "$INFO_FILE" ]; then
    scp "$BBB_HOST":"$INFO_FILE" "$LOCAL_LOG_DIR/kpi_rover_ecu.INFO" 2>/dev/null
fi

if [ -n "$WARNING_FILE" ]; then
    scp "$BBB_HOST":"$WARNING_FILE" "$LOCAL_LOG_DIR/kpi_rover_ecu.WARNING" 2>/dev/null
fi

if [ -n "$ERROR_FILE" ]; then
    scp "$BBB_HOST":"$ERROR_FILE" "$LOCAL_LOG_DIR/kpi_rover_ecu.ERROR" 2>/dev/null
fi

# Check if at least one file was fetched
if [ -f "$LOCAL_LOG_DIR/kpi_rover_ecu.INFO" ] || [ -f "$LOCAL_LOG_DIR/kpi_rover_ecu.WARNING" ] || [ -f "$LOCAL_LOG_DIR/kpi_rover_ecu.ERROR" ]; then
    echo "Log files copied successfully to $LOCAL_LOG_DIR/"
    echo ""
    echo "Available log files:"
    ls -lh "$LOCAL_LOG_DIR/"
else
    echo "Failed to fetch log files. Check your connection and verify the files exist on BBB."
    exit 1
fi
