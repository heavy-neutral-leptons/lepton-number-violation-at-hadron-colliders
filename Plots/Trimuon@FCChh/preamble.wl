(* Copyright 2023-2026 Bruno M. S. Oliveira *)

(* ::Package:: *)

(* ::Title:: *)

(*Contour Breaks*)

contoursWidth = Range[-7, 7, 1] // N

contoursNEvents = Range[1, 10, 1] // N

contoursSignificance = Log10[{2}] // N

contoursRll = Range[0.1, 0.9, 0.1] // N

contoursAsym = Range[-0.95, 0.95, 0.1] // N

(* ::Title:: *)

(* Custom ListContourPlot *)

RepeatedElements[list_, n_] :=
    Keys[Select[Counts[list], # >= n&]]

SetOptions[ListContourPlot, InterpolationOrder -> 1, ColorFunction ->
     "BlueGreenYellow", PlotLegends -> Automatic]

(*Export Points*)

NTo[x_?Internal`RealValuedNumericQ, p_Integer?Positive] :=
    Function[{m, e},
            N[Round[m 10^p] 10 ^ (e - p)]
        ] @@ MantissaExponent[x];

NTo[z_?NumericQ, p_Integer?Positive] :=
    Complex[NTo[Re[z], p], NTo[Im[z], p]];

NTo[p_Integer?Positive][x_] :=
    NTo[x, p];

exportpoints[plot_, name_] :=
    Module[{points},
        points = Flatten[Cases[Normal @ plot, Tooltip[contour_, label_
            ] :> Cases[contour, l_Line :> ArrayPad[First @ l, {{0}, {0, 1}}, label
            ]], Infinity], 1];
        points = Map[NTo[4], points, {3}];
        points = Riffle[points, {""}] // Flatten[#, 1]&;
        Export[name <> ".dat", points];
        points
    ]

(* ::Title:: *)

(*Make Plots*)

MhnlThetaSquaredWidthScan[data_, outPath_] :=
    Module[{plot},
        plot = ListContourPlot[data[All, {"meanMass", "thetaSquared",
             "meanWidth"}], ScalingFunctions -> {"Log10", "Log10", "Log10"}, Contours
             -> contoursWidth];
        exportpoints[plot, outPath];
        plot
    ]

MhnlThetaSquaredNEventsScan[data_, outPath_] :=
    Module[{plot},
        plot = ListContourPlot[data[All, {"meanMass", "thetaSquared",
             "nSignalEventsS"}], ScalingFunctions -> {"Log10", "Log10", "Log10"},
             Contours -> contoursNEvents];
        exportpoints[plot, outPath];
        plot
    ]

MhnlThetaSquaredScan[data_, outPath_] :=
    Module[{zExcludeSignal, zExcludeSuppression, rll},
        zExcludeSignal = ListContourPlot[data[All, {"meanMass", "thetaSquared",
             "zExcludeSignalTrunc"}], ScalingFunctions -> {"Log10", "Log10", "Log10"
            }, Contours -> contoursSignificance];
        exportpoints[zExcludeSignal, outPath <> "-zExcludeSignal"];
        zExcludeSuppression = ListContourPlot[data[All, {"meanMass",
            "thetaSquared", "zExcludeSuppressionTrunc"}], ScalingFunctions -> {"Log10",
             "Log10", "Log10"}, Contours -> contoursSignificance];
        exportpoints[zExcludeSuppression, outPath <> "-zExcludeSuppression"
            ];
        rll = ListContourPlot[data[All, {"meanMass", "thetaSquared",
            "lnv2lncRatio"}], ScalingFunctions -> {"Log10", "Log10", "Linear"}, Contours
             -> contoursRll, PlotRange -> All];
        exportpoints[rll, outPath <> "-rll"];
        GraphicsRow[{zExcludeSignal, zExcludeSuppression}]
    ]

AnalyseFixed\[CapitalDelta]m[name_, data_, deltam_] :=
    Module[{dataFiltered},
        dataFiltered = data[Select[#deltam == deltam&]];
        MhnlThetaSquaredScan[
            dataFiltered
            ,
            "data/contours/fixed-deltam/" <> name <> "-deltam" <>
                ToString[
                    ScientificForm[
                        deltam
                        ,
                        3
                        ,
                        ExponentFunction ->
                            (
                                If[# === Null,
                                    0
                                    ,
                                    #
                                ]&
                            )
                        ,
                        NumberFormat -> (Row[{#1, "e", #3}]&)
                    ]
                ]
        ]
    ]

AnalyseInverseSeesaw[name_, data_, mv_] :=
    Module[{dataFiltered},
        dataFiltered = data[Select[#inverseSeesawMass == mv&]];
        MhnlThetaSquaredScan[
            dataFiltered
            ,
            "data/contours/fixed-mnu/" <> name <> "-mnu" <>
                ToString[
                    ScientificForm[
                        mv
                        ,
                        3
                        ,
                        ExponentFunction ->
                            (
                                If[# === Null,
                                    0
                                    ,
                                    #
                                ]&
                            )
                        ,
                        NumberFormat -> (Row[{#1, "e", #3}]&)
                    ]
                ]
        ]
    ]
