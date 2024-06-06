import time
from datetime import datetime
import serial
import csv
import os

# Initialize serial connection
ser = serial.Serial('COM7', 9600)  # Adjust the COM port as needed
print("Serial connection established")

# Define the CSV file path
csv_file = 'CAPSTONEMAIN.csv'

# Check if the CSV file exists, if not, create a new one with headers
if not os.path.exists(csv_file):
    with open(csv_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Time', 'Date', 'Name', 'Branch', 'UID'])

# Read data from serial port
while True:
    try:
        # Read line from serial port
        line = ser.readline().decode('utf-8').strip()

        # Check if line is not empty
        if line:
            print("Received line:", line)  # Print received line for debugging

            # Get current timestamp
            timestamp = datetime.now().strftime('%H:%M:%S,%Y-%m-%d')

            # Extract UID Number
            if "UID Tag:" in line:
                uid_index = line.find("UID Tag:") + len("UID Tag:")
                uid_number = line[uid_index:].replace(" ", "").replace("0x", "").strip()
                uid_number = ','.join(uid_number[i:i+2] for i in range(0, len(uid_number), 2))
            elif "UID:" in line:
                uid_index = line.find("UID:") + len("UID:")
                uid_number = line[uid_index:].replace(" ", "").replace("0x", "").strip()
                uid_number = ','.join(uid_number[i:i+2] for i in range(0, len(uid_number), 2))
            else:
                uid_number = ''

            # Extract Name and Branch
            name = ""  # No specific name in this case
            branch = ""  # No specific branch in this case
            
            # Check if the line contains "Affan" or "Aqeel" and assign the corresponding values
            if "Affan" in line:
                name = "Affan"
                branch = "E.X.T.C."
                uid_number = "7391FC24"
            elif "Aqeel" in line:
                name = "Aqeel"
                branch = "I.T."
                uid_number = "E3F3220E"

            # Print data
            print(f"Time: {timestamp.split(',')[0]}, Date: {timestamp.split(',')[1]}, Name: {name}, Branch: {branch}, UID: {uid_number}")

            # Write data to CSV file only if there is valid data
            if name and branch and uid_number:
                with open(csv_file, 'a', newline='') as csvfile:
                    writer = csv.writer(csvfile)
                    writer.writerow([timestamp.split(',')[0], timestamp.split(',')[1], name, branch, f'"{uid_number}"'])

    except Exception as e:
        print(f"Error: {e}")

    time.sleep(0.1)  # Add a small delay to avoid high CPU usage
