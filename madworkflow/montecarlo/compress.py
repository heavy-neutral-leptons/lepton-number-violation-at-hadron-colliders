# Copyright 2023-2026 Bruno M. S. Oliveira

import gzip
import shutil
from pathlib import Path


def compress_data(input_path: Path, output_path: Path, remove_input: bool):
    if not input_path.exists():
        raise FileNotFoundError(f"Input file not found: {input_path}")

    if output_path.exists():
        raise FileExistsError(f"Output file already exists: {output_path}")

    with input_path.open("rb") as input:
        with gzip.open(output_path, "wb") as output:
            shutil.copyfileobj(input, output)

    print(f"Compressed {input_path} -> {output_path}")
    if remove_input:
        input_path.unlink()


def decompress_data(input_path: Path, output_path: Path):
    if not input_path.exists():
        raise FileNotFoundError(f"Input file not found: {input_path}")

    if output_path.exists():
        raise FileExistsError(f"Output file already exists: {output_path}")

    with gzip.open(input_path, "rb") as input:
        with open(output_path, "wb") as output:
            shutil.copyfileobj(input, output)

    print(f"Decompressed {input_path} -> {output_path}")
