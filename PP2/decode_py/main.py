import threading
import time
import signal
import sys
import os

WEIGHT = 100
HEIGHT = 100
TOTAL_PIXELS = WEIGHT * HEIGHT
TEST_KEYS = [88, 222, 5, 934]
NUM_THREADS = len(TEST_KEYS)

progress = [0] * NUM_THREADS
progress_lock = threading.Lock()

def try_test(signum, frame):
    progress_lock.acquire()
    print("Timer tick - Progress:")
    for i in range(NUM_THREADS):
        percentage = (progress[i] * 100.0) / TOTAL_PIXELS
        print(f"  Thread {i} (key {TEST_KEYS[i]}): {progress[i]} pixels decoded ({percentage:.2f}%)")
    print()
    progress_lock.release()

def decode_image(key_index):
    key = TEST_KEYS[key_index]
    
    # Small delay to ensure thread is registered by OS
    time.sleep(0.01)
    
    try:
        with open("encrypt.bin", "rb") as encrypt_file:
            os.makedirs("outputs", exist_ok=True)
            
            ppm_filename = f"outputs/output_{key}.ppm"
            txt_filename = f"outputs/output_{key}.txt"
            
            with open(ppm_filename, "wb") as ppm_file, \
                 open(txt_filename, "w") as txt_file:
                
                ppm_header = f"P6\n{WEIGHT} {HEIGHT}\n255\n"
                ppm_file.write(ppm_header.encode('ascii'))
                
                pixels_processed = 0
                while pixels_processed < TOTAL_PIXELS:
                    rgb_encrypted = encrypt_file.read(3)
                    if len(rgb_encrypted) != 3:
                        break
                    
                    component_R_decrypted = (rgb_encrypted[0] ^ key) & 0xFF
                    component_G_decrypted = (rgb_encrypted[1] ^ key) & 0xFF
                    component_B_decrypted = (rgb_encrypted[2] ^ key) & 0xFF
                    
                    rgb_bytes = bytes([component_R_decrypted, component_G_decrypted, component_B_decrypted])
                    ppm_file.write(rgb_bytes)
                    
                    txt_file.write(f"{component_R_decrypted} {component_G_decrypted} {component_B_decrypted}\n")
                    
                    time.sleep(0.0002)
                    
                    progress_lock.acquire()
                    progress[key_index] += 1
                    progress_lock.release()
                    
                    pixels_processed += 1
                    
    except FileNotFoundError:
        print(f"Error: 'encrypt.bin' not found.")
    except Exception as e:
        print(f"Error in thread for key {key}: {e}")

def main():
    try:
        with open("encrypt.bin", "rb"):
            pass
    except FileNotFoundError:
        print("Error: 'encrypt.bin' not found. Please create it first.")
        sys.exit(1)
    
    os.makedirs("outputs", exist_ok=True)
    
    signal.signal(signal.SIGALRM, try_test)
    
    signal.setitimer(signal.ITIMER_REAL, 0.1, 0.2)
    
    thread_list = []
    for i in range(NUM_THREADS):
        thread = threading.Thread(target=decode_image, args=(i,))
        thread_list.append(thread)
        thread.start()
    
    time.sleep(0.1)
    
    for thread in thread_list:
        thread.join()
    
    signal.setitimer(signal.ITIMER_REAL, 0, 0)
    
    print("Done!")

if __name__ == "__main__":
    main()
