import time

DURATION = 300
start_time = time.time()
elapsed = time.time() - start_time
while elapsed < DURATION :
    print(f"Time remaining: {DURATION - elapsed}")
    elapsed = time.time() - start_time