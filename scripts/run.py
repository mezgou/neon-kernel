import config
import platform
import sys
import os

from shell_utils import run_cmd


def setup_executable_file(build_dir: str) -> str:
    executable_file: str = os.path.join(build_dir, config.PROJECT_NAME)
    if platform.system().lower() == "windows":
        executable_file += ".exe"
    if not os.path.exists(executable_file):
        print(f"[Error] Executable file: {executable_file} not found")
        sys.exit(1)
    return executable_file


def main() -> None:
    arguments, remainder = config.parse_common_arguments()
    build_dir: str = os.path.join(config.BUILD_DIR, arguments.build_type)

    executable_file: str = setup_executable_file(build_dir)
    list_arguments = [executable_file] + remainder
    run_cmd(list_arguments)


if __name__ == "__main__":
    main()
