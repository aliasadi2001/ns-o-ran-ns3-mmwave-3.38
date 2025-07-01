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

def main():
    # Attach to System V shared memory for variables
    attempts = 5
    for i in range(attempts):
        try:
            print(f"Attempting to attach to shared memory key {SHM_KEY_VARS}...")
            shm_vars = sysv_ipc.SharedMemory(SHM_KEY_VARS)
            print(f"Successfully attached to shared memory key {SHM_KEY_VARS}")
            break
        except sysv_ipc.ExistentialError as e:
            if i == attempts - 1:
                print(f"Error: Shared memory key {SHM_KEY_VARS} not found after {attempts} attempts: {e}")
                return
            print(f"Shared memory key {SHM_KEY_VARS} not found, retrying in 2 seconds...")
            time.sleep(2)

    # Read shared variables as a Numpy array (no scalar indexing)
    shared_vars = np.frombuffer(shm_vars.read(), dtype=vars_dtype)
    # Create a writable copy of the array
    shared_vars_copy = np.copy(shared_vars)
    # shared_vars.flags.writeable = True

    # Access fields from the first (and only) element
    cpp_flag = shared_vars_copy[0]['cpp_flag']
    num_ues = shared_vars_copy[0]['num_ues']
    total_sym = shared_vars_copy[0]['total_sym']
    ue_data = [(shared_vars_copy[0]['ue_data'][i]['ueId'],
                shared_vars_copy[0]['ue_data'][i]['DLmax_sym'],
                shared_vars_copy[0]['ue_data'][i]['dl_MCS'])
               for i in range(num_ues)]

    print(f"Python: cpp_flag = {cpp_flag}")
    print(f"Python: num_ues = {num_ues}")
    print(f"Python: total_sym = {total_sym}")
    print(f"Python: ue_data = {ue_data}")

    # Create percentage list (example values)
    # percentages = [0.33, 0.33, 0.34][:num_ues]
    a = 100
    percentages = np.zeros(num_ues)
    for y in range(0,num_ues):
        percentages[y] = random.randint(0 , a)
        a  = a - percentages[y]

    # Attach to System V shared memory for percentages
    try:
        print(f"Attempting to attach to shared memory key {SHM_KEY_PERCENT}...")
        shm_percent = sysv_ipc.SharedMemory(SHM_KEY_PERCENT)
        print(f"Successfully attached to shared memory key {SHM_KEY_PERCENT}")
    except sysv_ipc.ExistentialError as e:
        print(f"Error: Shared memory key {SHM_KEY_PERCENT} not found: {e}")
        shm_vars.remove()
        return

    # Write percentages
    percentages_array = np.array(percentages, dtype=np.float64)
    shm_percent.write(percentages_array.tobytes())

    # Signal C++ that percentages are written
    shared_vars_copy[0]['py_flag'] = 1
    shm_vars.write(shared_vars_copy.tobytes()) # Write updated shared_vars back to shared memory

    print(f"Python: Wrote percentages = {percentages}")

    # Cleanup
    shm_vars.detach()
    shm_percent.detach()

if __name__ == "__main__":
    main()