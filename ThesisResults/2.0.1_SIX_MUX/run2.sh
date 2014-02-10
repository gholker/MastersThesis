#!/bin/sh

./MacPDNE cgp_pbil_bw_gm1.cfg cgp_pbil_bw_nogm1.cfg cgp_pbil_shc_gm1.cfg gep_pipe_bw_gm1.cfg cgp_pbil_shc_nogm1.cfg cgp_pipe_bw_gm1.cfg cgp_pipe_bw_nogm1.cfg gep_pipe_bw_nogm1.cfg > output1.log & 
./MacPDNE cgp_pipe_shc_gm1.cfg cgp_pipe_shc_nogm1.cfg gep_pbil_bw_gm1.cfg gep_pipe_shc_gm1.cfg gep_pbil_bw_nogm1.cfg gep_pbil_shc_gm1.cfg gep_pbil_shc_nogm1.cfg gep_pipe_shc_nogm1.cfg > output2.log &
