import subprocess
import platform
import sys
import os


BUILD_DIR: str = os.path.abspath("build")
ROOT_DIR: str = os.path.abspath(".")
PROJECT_NAME: str = "neon-kernel"


def run_cmd(cmd_list: list[str]) -> None:
    try:
        subprocess.run(cmd_list, check=True)
    except subprocess.CalledProcessError as exception:
        print(f"[Error] Command: {exception.cmd}")
        print(f"[Error] Return Code: {exception.returncode}")
        sys.exit(1)


def argument_handler() -> str:
    try:
        build_type: str = sys.argv[1]
        if build_type.lower() not in ["debug", "release"]:
            raise IndexError
    except IndexError:
        print("[Error] No build_type value entered, default=Release, next time input 'Release', 'Debug'")
        return "Release"
    return build_type.capitalize()


def setup_executable_file(build_dir: str) -> str:
    executable_file: str = os.path.join(build_dir, PROJECT_NAME)
    if platform.system().lower() == "windows":
        executable_file += ".exe"
    if not os.path.exists(executable_file):
        print(f"[Error] Executable file: {executable_file} not found")
        sys.exit(1)
    return executable_file


def main() -> None:
    build_type: str = argument_handler()
    build_dir: str = os.path.join(BUILD_DIR, build_type)

    run_cmd(["cmake", "-B", build_dir, "-S", ROOT_DIR, "-G", "Ninja", "-DCMAKE_BUILD_TYPE=" + build_type])
    run_cmd(["cmake", "--build", build_dir, "--parallel"])
    print("[Info] The build was completed successfully")

    executable_file: str = setup_executable_file(build_dir)
    run_cmd([executable_file])


if __name__ == "__main__":
    main()
