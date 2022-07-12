#!/usr/bin/env python2

input_files = {
    "toy_detector" : [
        #"rootfiles/result_26_layers_v3.root",
        #"rootfiles/result_alternative_thickness.root",
        #"rootfiles/result_26_layers_add_pcb_before_odd_active_layers_v3.root",
        #"rootfiles/result_26_layers_add_pcb_after_odd_active_layers_v3.root",

        "rootfiles/result_geometryType_0_run3_electron_nominal.root",
        "rootfiles/result_geometryType_1_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_2_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_3_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_4_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_5_run3_electron_nominal.root",
    ],

    "replace_PCB_by_Lead" : [
        "rootfiles/result_geometryType_0_run3_electron_nominal.root",
        "rootfiles/result_geometryType_1_run3_electron_nominal.root",
        "rootfiles/result_geometryType_4_run3_electron_nominal.root",
        "rootfiles/result_geometryType_5_run3_electron_nominal.root",
    ],

    "backwardHits_v1" : [
        #"rootfiles/result_geometryType_0_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_0_run3_electron_forward_hits.root",
        #"rootfiles/result_geometryType_0_run3_electron_nominal.root",
        "rootfiles/result_geometryType_1_run3_electron_nominal.root",
        "rootfiles/result_geometryType_1_run3_electron_forward_hits.root",
        "rootfiles/result_geometryType_1_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_2_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_2_run3_electron_forward_hits.root",
        #"rootfiles/result_geometryType_2_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_3_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_3_run3_electron_forward_hits.root",
        #"rootfiles/result_geometryType_3_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_4_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_4_run3_electron_forward_hits.root",
        #"rootfiles/result_geometryType_4_run3_electron_nominal.root",
        #"rootfiles/result_geometryType_5_run3_electron_backward_hits.root",
        #"rootfiles/result_geometryType_5_run3_electron_forward_hits.root",
        #"rootfiles/result_geometryType_5_run3_electron_nominal.root",
    ],

    "backwardHits_v2" : [
        "rootfiles/result_geometryType_2_run3_electron_nominal.root",
        "rootfiles/result_geometryType_2_run3_electron_forward_hits.root",
        "rootfiles/result_geometryType_2_run3_electron_backward_hits.root",
    ],
}

test_beam_result = {
    # layer depth is approximated from eyes
    "layer_depth" : [0.98  , 1.97  , 2.80  , 3.80  , 4.80  , 5.75  , 6.75    ,
                     7.75  , 8.60  , 9.60  , 10.50 , 11.50 , 12.40 , 13.40   ,
                     14.40 , 15.25 , 16.25 , 17.25 , 18.20 , 19.20 , 20.10   ,
                     21.25 , 22.20 , 23.40 , 24.25 , 25.60 , 26.50 , 27.80 ] ,

    "mips" : {
        "data" : {
                20: [ 27.97529793   , 56.94511032   , 109.28077698  , 136.3278656   , 185.52850342  , 183.97288513  , 190.09320068  ,
                    169.14602661    , 172.43197632  , 125.23709869  , 123.59194946  , 86.94829559   , 80.3841095    , 55.09900284   ,
                    49.03165436     , 30.44026566   , 28.33568954   , 18.1529274    , 21.78116608   , 9.80546951    , 7.89422131    ,
                    4.74877453      , 3.71464896    , 2.26393962    , 2.68312931    , 1.84798074    , 2.96802163    , 2.07398152]   ,
                100: [ 46.63975525  , 114.66628265  , 265.69363403  , 386.59579468  , 621.76507568  , 755.52404785  , 828.36853027  ,
                    850.06390381    , 961.36834717  , 762.60217285  , 810.68963623  , 615.45013428  , 609.97467041  , 436.13134766  ,
                    409.57852173    , 271.23117065  , 254.13935852  , 169.89068604  , 159.09002686  , 98.19945526   , 78.01111603   ,
                    48.89661789     , 38.81033707   , 23.93457222   , 23.25452232   , 14.55828953   , 13.5385313    , 8.12277794]   ,
                300: [  61.28678131 , 170.39982605  , 445.28921509  , 696.44915771  , 1232.91577148 , 1706.63549805 , 2004.63244629 ,
                    2117.43017578   , 2636.14453125 , 2165.76635742 , 2491.61181641 , 2010.32653809 , 2153.66015625 , 1509.47131348 ,
                    1532.20666504   , 1083.24182129 , 985.58740234  , 675.88018799  , 626.57421875  , 408.73651123  , 326.54840088  ,
                    208.37161255    , 165.11035156  , 102.31463623  , 98.6309967    , 62.07058716   , 52.98619843   , 30.69275284]
                },
        
        "mc" : {
                20: [ 21.50681686  , 52.96774292   , 105.72846985  , 139.59152222  , 190.9624176   , 188.5632019   , 212.36528015  ,
                    178.33622742   , 179.30996704  , 136.88346863  , 128.64570618  , 92.97825623   , 83.23855591   , 57.88684082   ,
                    50.43962479    , 34.07664871   , 29.20438576   , 19.41445923   , 16.39099503   , 10.66606712   , 7.91118002    ,
                    5.2383852      , 3.97883224    , 2.6746645     , 2.32645464    , 1.55437088    , 1.33136487    , 0.88430882]   ,
                100: [ 32.89741516 , 100.46727753  , 243.90615845  , 387.94775391  , 623.9732666   , 719.44714355  , 921.58850098  ,
                    876.69610596   , 977.50720215  , 823.13946533  , 837.11102295  , 652.08312988  , 622.23828125  , 457.28369141  ,
                    417.88778687   , 294.85919189  , 260.6965332   , 178.53686523  , 153.69523621  , 102.70782471  , 76.91766357   ,
                    50.60368347    , 38.71577835   , 25.14162064   , 21.87405968   , 13.90151215   , 11.96078873   , 7.24694681]   ,
                300: [  41.1002121 , 141.27975464  , 386.300354    , 688.69451904  , 1221.99047852 , 1552.28918457 , 2161.61791992 ,
                    2233.05151367  , 2661.48388672 , 2397.94360352 , 2579.23974609 , 2125.00439453 , 2119.68359375 , 1631.13183594 ,
                    1546.22912598  , 1129.73095703 , 1030.00244141 , 725.08172607  , 641.80291748  , 438.453125    , 335.5932312   ,
                    225.46757507   , 175.30892944  , 115.84326172  , 101.24755859  , 64.85153198   , 56.26319504   , 33.94129181]
                }
    },
    
    "multiplicity" : {
        "data" : {
            20: [ 1.5         , 3.          , 6.   , 6.         , 9.07692308  , 8.76923077 , 11.69230769 ,
                9.92307692    , 12.         , 9.   , 10.        , 7.34615385  , 8.         , 5.          ,
                5.            , 3.          , 3.   , 1.92307692 , 2.          , 1.         , 0.          ,
                0.            , 0.          , 0.   , 0.         , 0.          , 0.         , 0.        ] ,
            100: [ 2.90909091 , 4.          , 9.   , 10.        , 15.63636364 , 16.        , 22.         ,
                21.           , 26.         , 23.  , 27.        , 23.27272727 , 27.        , 22.         ,
                24.           , 17.54545455 , 19.  , 14.        , 14.09090909 , 9.         , 7.63636364  ,
                4.54545455    , 3.54545455  , 2.   , 2.         , 1.          , 1.         , 0.        ] ,
            300: [ 3.         , 5.          , 11.  , 13.2       , 20.         , 22.8       , 32.         ,
                31.           , 37.8        , 36.  , 42.        , 39.         , 43.8       , 40.4        ,
                44.           , 35.2        , 40.2 , 33.        , 35.         , 26.6       , 25.         ,
                17.8          , 15.         , 10.  , 10.        , 6.          , 5.         , 2.6]
            },
        
        "mc" : {
            20: [ 1.   , 2.   , 5.  , 5.   , 8.2  , 8.   , 11.  ,
                9.8    , 11.2 , 9.  , 10.  , 8.   , 8.   , 5.6  ,
                5.     , 3.   , 3.  , 2.   , 1.   , 1.   , 0.   ,
                0.     , 0.   , 0.  , 0.   , 0.   , 0.   , 0. ] ,
            100: [ 1.5 , 3.   , 7.  , 9.   , 14.  , 17.  , 21.  ,
                23.    , 26.  , 26. , 28.  , 25.  , 27.  , 23.  ,
                24.    , 19.  , 19. , 14.  , 13.6 , 9.   , 7.   ,
                5.     , 3.2  , 2.  , 2.   , 1.   , 1.   , 0. ] ,
            300: [ 2.  , 4.   , 9.  , 13.  , 19.  , 25.6 , 31.  ,
                36.    , 40.  , 43. , 46.2 , 45.  , 48.  , 44.  ,
                46.    , 41.  , 42. , 35.  , 35.  , 28.  , 25.  ,
                18.2   , 16.  , 11. , 10.  , 6.   , 5.   , 3. ]
        }
    }
}
