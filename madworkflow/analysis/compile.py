# Copyright 2023-2026 Bruno M. S. Oliveira

import subprocess

from ..common import analysis_dir


def compile():
    subprocess.run(
        [
            "cmake",
            "-DCMAKE_BUILD_TYPE=Release",
            "-B",
            analysis_dir,
            analysis_dir.parent,
        ],
        check=True,
    )

    subprocess.run(["cmake", "--build", analysis_dir], check=True)
