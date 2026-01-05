import subprocess
import sys


def run_cmd(cmd_list: list[str]) -> None:
    try:
        subprocess.run(cmd_list, check=True)
    except subprocess.CalledProcessError as exception:
        print(f"[Error] Command: {exception.cmd}")
        print(f"[Error] Return Code: {exception.returncode}")
        sys.exit(1)
        