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
    # Attach to System V shared memory for variables
    for i in range(attempts):
        try:
            print(f"Attempting to attach to shared memory key {key}...")
            shm = sysv_ipc.SharedMemory(key)
            print(f"Successfully attached to shared memory key {key}")
            return shm
        except sysv_ipc.ExistentialError as e:
            if i == attempts - 1:
                print(f"Error: Shared memory key {key} not found after {attempts} attempts: {e}")
                return None
            print(f"Shared memory key {key} not found, retrying in {delay} seconds...")
            time.sleep(delay)
    return None

def main():

    # Initial shared memory attachment
    shm_vars = attach_shared_memory(SHM_KEY_VARS)
    shm_percent = attach_shared_memory(SHM_KEY_PERCENT)
    if not shm_vars or not shm_percent:
        print("Failed to attach to shared memory. Exiting...")
        return
    # Track the last seen cpp_flag to detect updates
    last_cpp_flag  = -1
    try:
        while True:
   
            # Read shared variables as a Numpy array (no scalar indexing)
            shared_vars = np.frombuffer(shm_vars.read(), dtype=vars_dtype)
            # Create a writable copy of the array
            shared_vars_copy = np.copy(shared_vars)

            current_cpp_flag = shared_vars_copy[0]['cpp_flag']
            print(f"python: current C++ flag = {current_cpp_flag}")
            # Check if new data is available (cpp_flag has changed)
            if current_cpp_flag != last_cpp_flag:
                print(f"Python: Detected new data with cpp_flag = {current_cpp_flag}")
                last_cpp_flag = current_cpp_flag
                # Access fields from the first (and only) element
                # cpp_flag = shared_vars_copy[0]['cpp_flag']
                
                # Read shared variables
                num_ues = shared_vars_copy[0]['num_ues']
                total_sym = shared_vars_copy[0]['total_sym']
                ue_data = [(shared_vars_copy[0]['ue_data'][i]['ueId'],
                            shared_vars_copy[0]['ue_data'][i]['DLmax_sym'],
                            shared_vars_copy[0]['ue_data'][i]['dl_MCS'])
                            for i in range(num_ues)]

                # print(f"Python: cpp_flag = {cpp_flag}")
                print(f"Python: num_ues = {num_ues}")
                print(f"Python: total_sym = {total_sym}")
                print(f"Python: ue_data = {ue_data}")

                # Create percentage list (example values)
    # percentages = [0.33, 0.33, 0.34][:num_ues]
                a = 80
                percentages = np.zeros(num_ues)
                for y in range(0,num_ues):
                    percentages[y] = random.randint(0 , a)
                    a  = a - percentages[y]

    # Attach to System V shared memory for percentages
    # try:
        # print(f"Attempting to attach to shared memory key {SHM_KEY_PERCENT}...")
        # shm_percent = sysv_ipc.SharedMemory(SHM_KEY_PERCENT)
        # print(f"Successfully attached to shared memory key {SHM_KEY_PERCENT}")
    # except sysv_ipc.ExistentialError as e:
        # print(f"Error: Shared memory key {SHM_KEY_PERCENT} not found: {e}")
        # shm_vars.remove()
        # return

                # Write percentages
                percentages_array = np.array(percentages, dtype=np.float64)
                shm_percent.write(percentages_array.tobytes())
                print(f"Python: Wrote percentages = {percentages}")

                # Signal C++ that percentages are written
                shared_vars_copy[0]['py_flag'] = 1
                shm_vars.write(shared_vars_copy.tobytes()) # Write updated shared_vars back to shared memory

                print(f"Python: Wrote py_flag = {shared_vars_copy[0]['py_flag']}")

            #Sleep briefly to avoid excessive CPU usage
            time.sleep(0.1) # poll every 100ms

            # Check if shared memory is still valid
            try:
                shm_vars.read()
            except sysv_ipc.ExistentialError:
                print("Shared memory key 1234 was removed. Attempting to reattach...")
                shm_vars = attach_shared_memory(SHM_KEY_VARS)
                if not shm_vars:
                    print("Failed to reattach to shared memory key 1234. Exiting...")
                    break
                last_cpp_flag = -1  # Reset flag to detect new data

            try:
                shm_percent.read()
            except sysv_ipc.ExistentialError:
                print("Shared memory key 5678 was removed. Attempting to reattach...")
                shm_percent = attach_shared_memory(SHM_KEY_PERCENT)
                if not shm_percent:
                    print("Failed to reattach to shared memory key 5678. Exiting...")
                    break
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