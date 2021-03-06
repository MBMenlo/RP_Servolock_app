#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fpga_awg.h"
#include "main.h"
#include "rp.h"


/** AWG buffer length [samples]*/
#define n (16*1024)

/** AWG data buffer */
int32_t data[n];

rp_calib_params_t   g_osc_calib_params;

/** AWG FPGA parameters */
typedef struct {
    int32_t  offsgain;   ///< AWG offset & gain.
    uint32_t wrap;       ///< AWG buffer wrap value.
    uint32_t step;       ///< AWG step interval.
} awg_param_t;

/* Forward declarations */
void synthesize_signal(config_t &conf,
                       int32_t *data,
                       awg_param_t *params);
void write_data_fpga(uint32_t ch,
                     const int32_t *data,
                     const awg_param_t *awg);



/** Signal generator main */
int gen(config_t &conf)
{
  
    awg_param_t params;
    /* Prepare data buffer (calculate from input arguments) */
   
    synthesize_signal(conf, data, &params);
    /* Write the data to the FPGA and set FPGA AWG state machine */
    write_data_fpga(conf.ch, data, &params);
    return 0;
}


void synthesize_signal(config_t &conf,
                       int32_t *data,
                       awg_param_t *awg) {
    rp_CalibInit();
    g_osc_calib_params = rp_GetCalibrationSettings();


    uint32_t i;
    double ampl = conf.amp;
    double freq = conf.freq;
    signal_e type = conf.type;
    double endfreq = conf.end_freq;

    /* Various locally used constants - HW specific parameters */
    int dcoffs = 0;
    const int trans0 = 30;
    const int trans1 = 300;
    const double tt2 = 0.249;
 
    uint16_t scale = 0x1fff * (ampl / 2.0);
    uint32_t calib_gain = 0;
    if (conf.calib == true){
        calib_gain = (conf.ch == 0) ? g_osc_calib_params.be_ch1_fs : g_osc_calib_params.be_ch2_fs;
        dcoffs = (conf.ch == 0) ? g_osc_calib_params.be_ch1_dc_offs : g_osc_calib_params.be_ch2_dc_offs;
        float fullScale = (uint32_t) (1.0 / 100.0 * ((uint64_t)1<<32));
        scale *=   fullScale / (float)calib_gain;
    }

    scale = scale & 0x3fff;

    /* This is where frequency is used... */
    awg->offsgain = (dcoffs << 16) + scale;
    awg->step = round(65536 * freq/c_awg_smpl_freq * n);
    awg->wrap = round(65536 * n - 1);

    int trans = freq / 1e6 * trans1; /* 300 samples at 1 MHz */
    uint32_t amp = 8191;
    // uint32_t amp = ampl * 8191.0 / 2.0;    
    // if (amp > 8191) {
    //     /* Truncate to max value if needed */
    //     amp = 8191;
    // }

    if (trans <= 10) {
        trans = trans0;
    }


    /* Fill data[] with appropriate buffer samples */
    for(i = 0; i < n; i++) {
        
        /* Sine */
        if (type == eSignalSine) {
            data[i] = round(amp * cos(2*M_PI*(double)i/(double)n));
        }
 
        /* Square */
        if (type == eSignalSquare) {
            data[i] = round(amp * cos(2*M_PI*(double)i/(double)n));
            if (data[i] > 0)
                data[i] = amp;
            else 
                data[i] = -amp;

            /* Soft linear transitions */
            double mm, qq, xx, xm;
            double x1, x2, y1, y2;    

            xx = i;       
            xm = n;
            mm = -2.0*(double)amp/(double)trans; 
            qq = (double)amp * (2 + xm/(2.0*(double)trans));
            
            x1 = xm * tt2;
            x2 = xm * tt2 + (double)trans;
            
            if ( (xx > x1) && (xx <= x2) ) {  
                
                y1 = (double)amp;
                y2 = -(double)amp;
                
                mm = (y2 - y1) / (x2 - x1);
                qq = y1 - mm * x1;

                data[i] = round(mm * xx + qq); 
            }
            
            x1 = xm * 0.75;
            x2 = xm * 0.75 + trans;
            
            if ( (xx > x1) && (xx <= x2)) {  
                    
                y1 = -(double)amp;
                y2 = (double)amp;
                
                mm = (y2 - y1) / (x2 - x1);
                qq = y1 - mm * x1;
                
                data[i] = round(mm * xx + qq); 
            }
        }
        
        /* Triangle */
        if (type == eSignalTriangle) {
            data[i] = round(-1.0*(double)amp*(acos(cos(2*M_PI*(double)i/(double)n))/M_PI*2-1));
        }

        /* Sweep */
        /* Loops from i = 0 to n = 16*1024. Generates a sine wave signal that
           changes in frequency as the buffer is filled. */
        double start = 2 * M_PI * freq;
        double end = 2 * M_PI * endfreq;
        if (type == eSignalSweep) {
            double sampFreq = c_awg_smpl_freq; // 125 MHz
            double t = i / sampFreq; // This particular sample
            double T = n / sampFreq; // Wave period = # samples / sample frequency
            /* Actual formula. Frequency changes from start to end. */
            data[i] = round(amp * (sin((start*T)/log(end/start) * ((exp(t*log(end/start)/T)-1)))));
        }
        
        /* TODO: Remove, not necessary in C/C++. */
        if(data[i] < 0)
            data[i] += (1 << 14);
    }
}

/**
 * Write synthesized data[] to FPGA buffer.
 *
 * @param ch    Channel number [0, 1].
 * @param data  AWG data to write to FPGA.
 * @param awg   AWG paramters to write to FPGA.
 */
void write_data_fpga(uint32_t ch,
                     const int32_t *data,
                     const awg_param_t *awg) {

    uint32_t i;

    fpga_awg_init();

    if(ch == 0) {
        /* Channel A */
        g_awg_reg->cha_scale_off      = awg->offsgain;
        g_awg_reg->cha_count_wrap     = awg->wrap;
        g_awg_reg->cha_count_step     = awg->step;
        g_awg_reg->cha_start_off      = 0;

        for(i = 0; i < n; i++) {
            g_awg_cha_mem[i] = data[i];
        }
    } else {
        /* Channel B */
        g_awg_reg->chb_scale_off      = awg->offsgain;
        g_awg_reg->chb_count_wrap     = awg->wrap;
        g_awg_reg->chb_count_step     = awg->step;
        g_awg_reg->chb_start_off      = 0;

        for(i = 0; i < n; i++) {
            g_awg_chb_mem[i] = data[i];
        }
    }

    /* Enable both channels */
    /* TODO: Should this only happen for the specified channel?
     *       Otherwise, the not-to-be-affected channel is restarted as well
     *       causing unwanted disturbances on that channel.
     */ 
    g_awg_reg->state_machine_conf = 0x400040; // Reset State machine
    g_awg_reg->state_machine_conf = 0x010001;

    fpga_awg_exit();
}
