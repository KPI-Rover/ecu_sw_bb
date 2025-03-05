cmake_minimum_required(VERSION 3.10)
project(librobotcontrol_mock)

# Check if target already exists before creating it
if(NOT TARGET robotcontrol_mock AND 
   NOT TARGET local_robotcontrol_mock AND 
   NOT TARGET kpi_rover_robotcontrol_mock)
  
  # Create uniquely named target
  add_library(kpi_rover_robotcontrol_mock STATIC
      src/gpio.c
      src/motor.c
      src/encoder.c
      src/process_control.c
  )

  # Set target properties
  set_target_properties(kpi_rover_robotcontrol_mock PROPERTIES
      OUTPUT_NAME "robotcontrol_mock"  # This keeps the output library filename the same
  )

  # Add include directories
  target_include_directories(kpi_rover_robotcontrol_mock
      PUBLIC
          ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

  # Install rules if needed
  install(TARGETS kpi_rover_robotcontrol_mock
      ARCHIVE DESTINATION lib
  )
else()
  # If the target already exists, inform with a status message
  message(STATUS "Skipping creation of robotcontrol_mock - target already exists")
endif()
