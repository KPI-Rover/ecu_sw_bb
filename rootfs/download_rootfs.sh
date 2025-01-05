#!/bin/bash

# Variables
FILE_ID="1O40G2JT2BrE1PQ5OXNXle2qoqNJlNF09"
OUTPUT_FILE="rootfs.tar.xz"
EXTRACT_DIR="rootfs"

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Ensure gdown is installed
if ! command_exists gdown; then
    echo "gdown is not installed. Installing..."
    pip install gdown || { echo "Failed to install gdown. Exiting."; exit 1; }
fi

# Download the file from Google Drive
echo "Downloading rootfs from Google Drive..."
gdown "https://drive.google.com/uc?id=${FILE_ID}" -O "${OUTPUT_FILE}"

# Check if the file was downloaded successfully
if [[ ! -f "${OUTPUT_FILE}" ]]; then
    echo "Download failed. Exiting."
    exit 1
fi

echo "Download completed: ${OUTPUT_FILE}"

# Extract the tar.xz file
echo "Extracting ${OUTPUT_FILE}..."
mkdir -p "${EXTRACT_DIR}"
tar -xf "${OUTPUT_FILE}" -C "${EXTRACT_DIR}"

# Check if extraction was successful
if [[ $? -eq 0 ]]; then
    echo "Extraction completed. Files are in: ${EXTRACT_DIR}"
else
    echo "Extraction failed. Exiting."
    exit 1
fi

# Remove the downloaded tar file
echo "Removing the downloaded tar file: ${OUTPUTFILE}"
rm -f "${OUTPUT_FILE}"

if [[ $? -eq 0 ]]; then
    echo "Downloaded tar file removed successfully."
else
    echo "Failed to remove the downloaded tar file. Please check manually."
    exit 1
fi

echo "Script completed successfully."

