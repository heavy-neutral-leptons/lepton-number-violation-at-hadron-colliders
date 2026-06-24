#!/bin/bash
# Copyright 2023-2026 Bruno M. S. Oliveira

set -e

DELPHES_BIN="$MG5_HOME/Delphes/DelphesLHEF"
export DELPHES_BIN

DELPHES_CARD="./Cards/Delphes/delphes_card_empty.tcl"
export DELPHES_CARD

TAG="tag_1"
export TAG

process_run() {
    RUN_DIR="$1"

    OUTPUT_FILENAME="$RUN_DIR/$TAG""_delphes_events.root"
    LOG_FILENAME="$RUN_DIR/$TAG""_delphes.log"

    if ! [ -e "$OUTPUT_FILENAME" ]; then
        COMPRESSED_DATA_FILENAME="$RUN_DIR/unweighted_events.lhe.gz"

        echo "Running Delphes detector simulation on $COMPRESSED_DATA_FILENAME into $OUTPUT_FILENAME ($LOG_FILENAME)"
        gunzip --keep --stdout "$COMPRESSED_DATA_FILENAME" | "$DELPHES_BIN" "$DELPHES_CARD" "$OUTPUT_FILENAME" >"$LOG_FILENAME" 2>&1
        echo "Finished $OUTPUT_FILENAME"
    else
        echo "$OUTPUT_FILENAME already exists"
    fi
}
export -f process_run

for RUN_DIR in "$@"; do
    echo "$RUN_DIR"
    find "$RUN_DIR/Events" -name "run_*" -type d | parallel --jobs 40 process_run
done
