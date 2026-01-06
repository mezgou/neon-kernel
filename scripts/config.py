import argparse
import os


BUILD_DIR: str = os.path.abspath("build")
ROOT_DIR: str = os.path.abspath(".")
PROJECT_NAME: str = "neon-kernel"


def parse_common_arguments() -> tuple[argparse.Namespace, list[str]]:
    parser = argparse.ArgumentParser(description="neon-kernel Build/Run Tool")
    parser.add_argument(
        "--build-type",
        nargs="?",
        default="Release",
        type=str.capitalize,
        choices=["Debug", "Release"],
        help="Type of build (default: Release)"
    )
    return parser.parse_known_args()
