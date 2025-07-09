import sysv_ipc
import numpy as np
import time
import random

# Constants matching C++ code
MAX_UES = 12
SHM_KEY_VARS = 1234
SHM_KEY_PERCENT = 5678

# Struct layout for shared variables (matching C++ struct SharedVars)
vars_dtype = np.dtype([
    ('cpp_flag', np.int32),
    ('py_flag', np.int32),
    ('num_ues', np.int32),
    ('total_sym', np.int32),
    ('ue_data', np.dtype([
        ('ueId', np.int32),
        ('DLmax_sym', np.int32),
        ('dl_MCS', np.int32)
    ]), MAX_UES)
])

def attach_shared_memory(key, attempts=5, delay=2):
    """Helper function to attach to shared memory with retries."""
    for i in range(attempts):
        try:
            print(f"Attempting to attach to shared memory key {key}...")
            shm = sysv_ipc.SharedMemory(key)
            shmid = shm.id  # Get the shmid for debugging
            print(f"Successfully attached to shared memory key {key}, shmid = {shmid}")
            return shm
        except sysv_ipc.ExistentialError as e:
            if i == attempts - 1:
                print(f"Error: Shared memory key {key} not found after {attempts} attempts: {e}")
                return None
            print(f"Shared memory key {key} not found, retrying in {delay} seconds...")
            time.sleep(delay)
    return None

def main():
    # Track the last seen cpp_flag and timestamp to detect stalls
    last_cpp_flag = -1
    last_flag_change_time = time.time()
    STALL_TIMEOUT = 0.3  # Seconds to wait before suspecting a new shm segment
    shm_vars = None
    shm_percent = None

    try:
        while True:
            # Ensure shared memory is attached
            if not shm_vars:
                shm_vars = attach_shared_memory(SHM_KEY_VARS)
                if not shm_vars:
                    print("Failed to attach to shared memory key 1234. Retrying in main loop...")
                    time.sleep(2)
                    continue
                last_cpp_flag = -1  # Reset flag on reattachment
                last_flag_change_time = time.time()

            if not shm_percent:
                shm_percent = attach_shared_memory(SHM_KEY_PERCENT)
                if not shm_percent:
                    print("Failed to attach to shared memory key 5678. Retrying in main loop...")
                    time.sleep(2)
                    continue

            try:
                # Read shared variables as a Numpy array
                shared_vars = np.frombuffer(shm_vars.read(), dtype=vars_dtype)
                # Create a writable copy of the array
                shared_vars_copy = np.copy(shared_vars)

                current_cpp_flag = shared_vars_copy[0]['cpp_flag']
                print(f"Python: current C++ flag = {current_cpp_flag}, shmid = {shm_vars.id}")

                # Check if new data is available (cpp_flag has changed)
                if current_cpp_flag != last_cpp_flag:
                    print(f"Python: Detected new data with cpp_flag = {current_cpp_flag}, shmid = {shm_vars.id}")
                    last_cpp_flag = current_cpp_flag
                    last_flag_change_time = time.time()

                    # Read shared variables
                    num_ues = shared_vars_copy[0]['num_ues']
                    total_sym = shared_vars_copy[0]['total_sym']
                    ue_data = [(shared_vars_copy[0]['ue_data'][i]['ueId'],
                                shared_vars_copy[0]['ue_data'][i]['DLmax_sym'],
                                shared_vars_copy[0]['ue_data'][i]['dl_MCS'])
                               for i in range(num_ues)]

                    print(f"Python: num_ues = {num_ues}")
                    print(f"Python: total_sym = {total_sym}")
                    print(f"Python: ue_data = {ue_data}")

                    # Generate percentages that sum to 100
                    if num_ues > 0:
                        percentages = []
                        total = 100
                        for i in range(num_ues - 1):
                            percent = random.randint(0, total)
                            percentages.append(percent)
                            total -= percent
                        percentages.append(total)
                        percentages = np.array([p / 100.0 for p in percentages], dtype=np.float64)
                    else:
                        percentages = np.array([], dtype=np.float64)

                    # Write percentages
                    try:
                        shm_percent.write(percentages.tobytes())
                        print(f"Python: Wrote percentages = {percentages}, shmid = {shm_percent.id}")
                    except sysv_ipc.ExistentialError:
                        print("Shared memory key 5678 was removed during write. Attempting to reattach...")
                        shm_percent.detach()
                        shm_percent = None
                        continue

                    # Signal C++ that percentages are written
                    shared_vars_copy[0]['py_flag'] = 1
                    try:
                        shm_vars.write(shared_vars_copy.tobytes())
                        print(f"Python: Wrote py_flag = {shared_vars_copy[0]['py_flag']}, shmid = {shm_vars.id}")
                    except sysv_ipc.ExistentialError:
                        print("Shared memory key 1234 was removed during write. Attempting to reattach...")
                        shm_vars.detach()
                        shm_vars = None
                        continue

                # Check for stall (cpp_flag unchanged for too long)
                if time.time() - last_flag_change_time > STALL_TIMEOUT:
                    print(f"Python: No change in cpp_flag for {STALL_TIMEOUT} seconds. Attempting to reattach to shared memory...")
                    shm_vars.detach()
                    shm_percent.detach()
                    shm_vars = None
                    shm_percent = None
                    last_cpp_flag = -1
                    last_flag_change_time = time.time()
                    continue

                # Sleep briefly to avoid excessive CPU usage
                time.sleep(0.1)  # Poll every 100ms

            except sysv_ipc.ExistentialError:
                print(f"Shared memory key 1234 was removed (shmid = {shm_vars.id if shm_vars else 'unknown'}). Attempting to reattach...")
                shm_vars.detach()
                shm_vars = None
                last_cpp_flag = -1
                last_flag_change_time = time.time()
                continue

    except KeyboardInterrupt:
        print("Python: Shutting down...")
    finally:
        # Cleanup
        if shm_vars:
            shm_vars.detach()
        if shm_percent:
            shm_percent.detach()

if __name__ == "__main__":
    main()
