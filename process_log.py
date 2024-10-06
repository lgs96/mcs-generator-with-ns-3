import csv
from collections import defaultdict
import os
import numpy as np

# Create output directory if it doesn't exist
output_folder = "output_logs"
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Function to process the log file and generate MCS and SINR logs for each RNTI
def process_ns3_log(file_name):
    ue_data = defaultdict(lambda: {"mcs": [], "sinr": []})

    # First, count the total number of lines in the file to calculate progress
    with open(file_name, 'r') as file:
        total_lines = sum(1 for _ in file)

    # Now process the file and show progress
    processed_lines = 0
    last_time_logged = 0.1  # Start logging after 0.1 second

    with open(file_name, 'r') as file:
        for line in file:
            processed_lines += 1

            # Skip header or invalid lines that don't contain numerical data
            if line.startswith("DL"):
                parts = line.strip().split()

                # Ensure there are enough parts and process only valid lines
                if len(parts) < 14:
                    continue

                try:
                    # Extract the relevant fields
                    time = float(parts[1])  # Time (seconds)
                    rnti = int(parts[8])     # RNTI (UE identifier)
                    mcs = int(parts[11])     # MCS value
                    sinr = float(parts[13])  # SINR (dB)

                    # Log data only after 0.1 seconds and every 2.5 ms
                    if time >= 0.1 and time >= last_time_logged + 0.0025:
                        # Store MCS and SINR for the corresponding RNTI
                        ue_data[rnti]["mcs"].append(mcs)
                        ue_data[rnti]["sinr"].append(sinr)
                        last_time_logged = time

                except ValueError:
                    continue  # Ignore lines with conversion errors

            # Optionally, print progress
            if processed_lines % 1000 == 0:
                progress = (processed_lines / total_lines) * 100
                print(f"Processing... {progress:.2f}% completed.", end = '\r')

    # Save individual MCS and SINR files for each RNTI
    for rnti, data in ue_data.items():
        # Create and save the MCS file
        mcs_file = os.path.join(output_folder, f"mcs{rnti}.csv")
        with open(mcs_file, 'w', newline='') as mcs_csv:
            writer = csv.writer(mcs_csv)
            writer.writerow(["MCS"])
            writer.writerows([[mcs] for mcs in data["mcs"]])

        # Create and save the SINR file
        sinr_file = os.path.join(output_folder, f"sinr{rnti}.csv")
        with open(sinr_file, 'w', newline='') as sinr_csv:
            writer = csv.writer(sinr_csv)
            writer.writerow(["SINR"])
            writer.writerows([[sinr] for sinr in data["sinr"]])

    # Save the summary for each RNTI
    summary_file = os.path.join(output_folder, "summary.txt")
    with open(summary_file, 'w') as summary_txt:
        for rnti in sorted(ue_data.keys()):
            # Retrieve MCS and SINR data
            mcs_values = ue_data[rnti]["mcs"]
            sinr_values = ue_data[rnti]["sinr"]
            length = len(mcs_values)

            # Calculate average and std for MCS and SINR
            mcs_avg = np.mean(mcs_values) if mcs_values else 0
            sinr_avg = np.mean(sinr_values) if sinr_values else 0
            mcs_std = np.std(mcs_values) if mcs_values else 0
            sinr_std = np.std(sinr_values) if sinr_values else 0

            # Write summary to the summary file
            summary_txt.write(f"RNTI {rnti}:\n")
            summary_txt.write(f"  MCS - Length: {length}, Avg: {mcs_avg:.2f}, Std: {mcs_std:.2f}\n")
            summary_txt.write(f"  SINR - Avg: {sinr_avg:.2f}, Std: {sinr_std:.2f}\n\n")

            # Print the same information to the terminal
            print(f"RNTI {rnti} Length: {length}, MCS Avg: {mcs_avg:.2f}, MCS Std: {mcs_std:.2f}, SINR Avg: {sinr_avg:.2f}, SINR Std: {sinr_std:.2f}")

    print(f"Summary generated in {summary_file}")

# Process the log file
process_ns3_log("RxPacketTrace.txt")
