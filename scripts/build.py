import config
import os

from shell_utils import run_cmd


def main() -> None:
    build_type: str = config.parse_common_arguments()[0].build_type
    build_dir: str = os.path.join(config.BUILD_DIR, build_type)

    run_cmd(["cmake", "-B", build_dir, "-S", config.ROOT_DIR, "-G", "Ninja", "-DCMAKE_BUILD_TYPE=" + build_type])
    run_cmd(["cmake", "--build", build_dir, "--parallel"])
    print("[Info] The build was completed successfully")


if __name__ == "__main__":
    main()
