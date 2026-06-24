#!/bin/bash
# Copyright 2023-2026 Bruno M. S. Oliveira

set -e

DEFAULT_SIMULATIONS="Trimuon@FCChh/SPSS_251209a-dy+vbf"

DELPHES_PYTHIA8_BIN="$MG5_HOME/Delphes/DelphesPythia8"
export DELPHES_PYTHIA8_BIN

DELPHES_HEPMC2_BIN="$MG5_HOME/Delphes/DelphesHepMC2"
export DELPHES_HEPMC2_BIN

DEFAULT_PYTHIA_CARD="./Cards/Pythia8/configLHE.cmnd"
export DEFAULT_PYTHIA_CARD

DELPHES_CARD="./Cards/Delphes/FCChh/FCChh.tcl"
export DELPHES_CARD

TAG="tag_1"
export TAG

process_run() {
    RUN_DIR="$1"
    RUN=$(basename "$RUN_DIR")

    MADGRAPH_DATA_FILENAME="$RUN_DIR/unweighted_events.lhe"
    COMPRESSED_MADGRAPH_DATA_FILENAME="$MADGRAPH_DATA_FILENAME.gz"

    PYTHIA_DATA_FILENAME="$RUN_DIR/$TAG""_pythia8_events.hepmc"
    COMPRESSED_PYTHIA_DATA_FILENAME="$PYTHIA_DATA_FILENAME.gz"

    PYTHIA_CARD="$RUN_DIR/$TAG""_pythia8_card.dat"

    OUTPUT_FILENAME="$RUN_DIR/$TAG""_delphes_events.root"
    LOG_FILENAME="$RUN_DIR/$TAG""_delphes.log"

    if [ -e "$OUTPUT_FILENAME" ]; then
        echo "$OUTPUT_FILENAME already exists"
    elif [ -e "$PYTHIA_DATA_FILENAME" ]; then
        echo "$PYTHIA_DATA_FILENAME is still present"
    elif [ -e "$COMPRESSED_PYTHIA_DATA_FILENAME" ]; then
        echo "Running Delphes on $COMPRESSED_PYTHIA_DATA_FILENAME to produce $OUTPUT_FILENAME ($LOG_FILENAME)"

        gunzip --keep --stdout "$COMPRESSED_PYTHIA_DATA_FILENAME" |
            "$DELPHES_HEPMC2_BIN" "$DELPHES_CARD" "$OUTPUT_FILENAME" >"$LOG_FILENAME" 2>&1

        rm -v "$COMPRESSED_PYTHIA_DATA_FILENAME"

        echo "Finished producing $OUTPUT_FILENAME"
    else
        echo "Running Pythia+Delphes on $COMPRESSED_MADGRAPH_DATA_FILENAME to produce $OUTPUT_FILENAME ($LOG_FILENAME)"

        gunzip --keep "$COMPRESSED_MADGRAPH_DATA_FILENAME"

        NEVENTS=$(grep nevents "$RUN_DIR/$RUN""_""$TAG""_banner.txt" | sed -e 's/^ *//' -e 's/ *= nevents.*//')

        sed \
            -e "s#Main:numberOfEvents =#Main:numberOfEvents = $NEVENTS#" \
            -e "s#Beams:LHEF =#Beams:LHEF = $MADGRAPH_DATA_FILENAME#" \
            $DEFAULT_PYTHIA_CARD >"$PYTHIA_CARD"

        "$DELPHES_PYTHIA8_BIN" "$DELPHES_CARD" "$PYTHIA_CARD" "$OUTPUT_FILENAME" >"$LOG_FILENAME" 2>&1

        rm -v "$MADGRAPH_DATA_FILENAME"

        echo "Finished producing $OUTPUT_FILENAME"
    fi
}
export -f process_run

if [ $# -eq 1 ]; then
    find "$1.run/Events" -name "run_*" -type d | parallel --line-buffer --jobs 40 process_run
else
    find "$DEFAULT_SIMULATIONS-"*".run/Events" -name "run_*" -type d | parallel --line-buffer --jobs 40 process_run
fi
