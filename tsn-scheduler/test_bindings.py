import sys
import os

# Add build path to sys.path (Simple heuristic for development)
# In real scenario, we'd install it. Here we look for .pyd in current dir
sys.path.append(os.getcwd())

try:
    import tsn_scheduler
    print("[PASS] Import tsn_scheduler")
except ImportError as e:
    print(f"[FAIL] Import tsn_scheduler: {e}")
    sys.exit(1)

# Test API
try:
    if tsn_scheduler.create():
        print("[PASS] tsn_scheduler.create()")
    else:
        print("[FAIL] tsn_scheduler.create() returned False")
        
    tsn_scheduler.apply_config([1, 2, 3]) # Mock config
    print("[PASS] tsn_scheduler.apply_config()")

    tsn_scheduler.destroy()
    print("[PASS] tsn_scheduler.destroy()")

except Exception as e:
    print(f"[FAIL] Exception during test: {e}")
    sys.exit(1)
