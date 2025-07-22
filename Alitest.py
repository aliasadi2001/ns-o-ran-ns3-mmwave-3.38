import time
import random
import os

# Define the control file name (must match the one in your C++ code)
ML_CONTROL_FILE = "ml_traffic_control.txt"

# Define ranges for random values for each slice
# These are the parameters your ML expert would output
URLLC_PACKET_SIZE_RANGE = (40, 80)        # Bytes for URLLC packets
URLLC_INTERVAL_US_RANGE = (100, 300)      # Microseconds interval for URLLC

EMBB_PACKET_SIZE_RANGE = (1000, 6000)     # Bytes for eMBB packets
EMBB_INTERVAL_US_RANGE = (500, 2000)      # Microseconds interval for eMBB (0.5ms to 2ms)

MMTC_PACKET_SIZE_RANGE = (10, 30)         # Bytes for mMTC packets
MMTC_INTERVAL_US_RANGE = (500000, 2000000) # Microseconds interval for mMTC (0.5s to 2s)

def write_parameters_to_file():
    """
    Generates random traffic parameters for each slice and writes them to the control file.
    Uses an atomic write operation to ensure the C++ simulation always reads a complete file.
    """
    # 1. Generate (or calculate via ML) the new parameters
    urllc_ps = random.randint(*URLLC_PACKET_SIZE_RANGE)
    urllc_int = random.randint(*URLLC_INTERVAL_US_RANGE)

    embb_ps = random.randint(*EMBB_PACKET_SIZE_RANGE)
    embb_int = random.randint(*EMBB_INTERVAL_US_RANGE)

    mmtc_ps = random.randint(*MMTC_PACKET_SIZE_RANGE)
    mmtc_int = random.randint(*MMTC_INTERVAL_US_RANGE)

    # 2. Format the parameters into a string (key=value pairs)
    content = f"""urllc_packet_size={urllc_ps}
urllc_interval_us={urllc_int}
embb_packet_size={embb_ps}
embb_interval_us={embb_int}
mmtc_packet_size={mmtc_ps}
mmtc_interval_us={mmtc_int}
"""
    # 3. Write to a temporary file and then rename it
    # This ensures an atomic write: the C++ side either reads the old file or the complete new one.
    temp_file = ML_CONTROL_FILE + ".tmp"
    try:
        with open(temp_file, "w") as f:
            f.write(content)
        os.replace(temp_file, ML_CONTROL_FILE) # Atomically replaces the old file with the new one
        print(f"[{time.time():.2f}] Python Controller: Wrote new parameters to {ML_CONTROL_FILE}")
        print(f"  URLLC: PacketSize={urllc_ps}, Interval={urllc_int}us")
        print(f"  eMBB: PacketSize={embb_ps}, Interval={embb_int}us")
        print(f"  mMTC: PacketSize={mmtc_ps}, Interval={mmtc_int}us")
    except IOError as e:
        print(f"[{time.time():.2f}] Python Controller Error writing file: {e}")

if __name__ == "__main__":
    print(f"Python Controller started. Writing parameters to '{ML_CONTROL_FILE}' every 10 seconds.")
    # Write initial parameters immediately
    write_parameters_to_file()
    
    # Loop to write parameters periodically
    while True:
        time.sleep(10) # Wait for 10 real-world seconds
        write_parameters_to_file()
