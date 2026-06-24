# Copyright 2023-2026 Bruno M. S. Oliveira
################################################################################
# Production@FCChh                                                             #
################################################################################
# ----- Neutral Current -----
if false; then
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-nc-qq &
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-nc-gq &
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-nc-aq &
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_260121a-nc-gg &
fi

# ----- Charged Current -----
if false; then
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-cc-qq &
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-cc-gq &
    python madproduce.py --runparton --runlhe2root --study Production@FCChh --name SPSS_251209a-cc-aq &
fi

wait

################################################################################
# LeptonNumber@FCChh                                                           #
################################################################################
STUDY="LeptonNumber@FCChh"
if false; then
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-ms-all &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-ms-lnc &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-ms-lnv &

    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-os-all &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-os-lnc &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-os-lnv &

    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-sc-all &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-sc-lnc &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-sc-lnv &

    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-np-all &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-np-lnc &
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-np-lnv &

    wait
fi

################################################################################
# {Trimuon,DimuonSS}@FCChh                                                   #
################################################################################
STUDIES=("Trimuon@FCChh" "DimuonSS@FCChh")

# ----- Flagship Simulations -----
for STUDY in "${STUDIES[@]}"; do
    if true; then
        python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq
    fi
done

# ----- MadGraph5_aMC@NLO Syntaxes -----
for STUDY in "${STUDIES[@]}"; do
    if false; then
        # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-ms &
        python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-os &
        # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-sc &
        # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name SPSS_251209a-qq+gq+aq-np &

        wait
    fi
done

# ----- Background -----
STUDY="Trimuon@FCChh"
if false; then
    # rm -rf "$STUDY/bg-4l.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-4l &

    # rm -rf "$STUDY/bg-3lv.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-3lv &

    # rm -rf "$STUDY/bg-wwll.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-wwll &

    # rm -rf "$STUDY/bg-3w.run" "$STUDY/bg-3w-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-3w &

    # rm -rf "$STUDY/bg-ttll.run" "$STUDY/bg-ttll-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttll &

    # rm -rf "$STUDY/bg-ttlv.run" "$STUDY/bg-ttlv-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttlv &

    # rm -rf "$STUDY/bg-tqll.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-tqll &

    wait

    grep 'Nb of events' "$STUDY/bg"*".log" | sort | uniq | cut -d ':' -f1,3 | sort -k 2
fi

STUDY="DimuonSS@FCChh"
if false; then
    # rm -rf "$STUDY/bg-4l.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-4l &

    # rm -rf "$STUDY/bg-3lv.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-3lv &

    # rm -rf "$STUDY/bg-wwll.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-wwll &

    # rm -rf "$STUDY/bg-3w.run" "$STUDY/bg-3w-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-3w &
    # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-3w-jet1 &

    # rm -rf "$STUDY/bg-ttll.run" "$STUDY/bg-ttll-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttll &
    # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttll-jet1 &

    # rm -rf "$STUDY/bg-ttlv.run" "$STUDY/bg-ttlv-"[0-9][0-9]".run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttlv &
    # python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-ttlv-jet1 &

    # rm -rf "$STUDY/bg-tqll.run"
    python madproduce.py --runparton --runlhe2root --rundetector --study "$STUDY" --name bg-tqll &

    wait

    grep 'Nb of events' "$STUDY/bg"*".log" | sort | uniq | cut -d ':' -f1,3 | sort -k 2
fi
