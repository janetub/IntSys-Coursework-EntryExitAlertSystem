# AI-Assisted Entry and Exit Alert System

A C program that monitors and logs entry ('N') and exit ('X') activities, calculates thresholds for unusual inactivity, and triggers an SOS alert.

## Features
- **Activity Tracking**: Logs entries and exits with timestamps.
- **Threshold Calculation**: Analyzes activity patterns to set thresholds.
- **Alert System**: Initiates an SOS signal when thresholds are exceeded.
- **Log Maintenance**: Manages activity records in a linked list.

## Functions
- `add_activity()`: Inserts a new activity into the log.
- `calculate_in_threshold()`: Sets the threshold for time spent inside.
- `calculate_out_threshold()`: Sets the threshold for time spent outside.
- `check_unusual_activity()`: Detects and alerts on unusual activity.
- `printList()`: Displays the activity log.

## Usage
Compile and run the program, then use the interactive menu to log activities and view the log. A background thread continuously monitors for unusual activity.