# Copyright 2023-2026 Bruno M. S. Oliveira

import os
import subprocess
from pathlib import Path

import tqdm.contrib.concurrent

FNULL = open(os.devnull, "w")
max_workers: int = min(64, (os.cpu_count() or 0) + 4)


def save_cmds(cmds: list[list[str | Path]], output_path: Path):
    with open(output_path, "w") as f:
        for cmd in cmds:
            f.write(" ".join(map(str, cmd)) + "\n")


def run_function(fn, iterables, max_workers=max_workers):
    return tqdm.contrib.concurrent.process_map(fn, iterables, max_workers=max_workers)


def run_cmd(cmd: list[str | Path], stdout=FNULL, stderr=FNULL):
    subprocess.run(cmd, stdout=stdout, stderr=stderr)


def run_cmds(cmds: list[list[str | Path]], max_workers=max_workers):
    return tqdm.contrib.concurrent.process_map(run_cmd, cmds, max_workers=max_workers)


def run_cmd_row(analysis_cmd: list[str | Path], stderr=FNULL):
    output = subprocess.check_output(analysis_cmd, stderr=stderr)

    output = output.decode("utf-8")
    output = output.strip()

    output = output.split(",")

    return output


def run_cmds_row(cmds: list[list[str | Path]], max_workers=max_workers):
    return tqdm.contrib.concurrent.process_map(
        run_cmd_row, cmds, max_workers=max_workers
    )


def run_cmd_table(analysis_cmd: list[str | Path], stderr=FNULL):
    output = subprocess.check_output(analysis_cmd, stderr=stderr)

    output = output.decode("utf-8")
    output = output.strip()

    output = output.split("\n")

    output = [row.split(",") for row in output]

    return output


def run_cmds_table(cmds: list[list[str | Path]], max_workers=max_workers):
    return tqdm.contrib.concurrent.process_map(
        run_cmd_table, cmds, max_workers=max_workers
    )
