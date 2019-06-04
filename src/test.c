#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "arm_const_structs.h"
#include "audioMoth.h"
#include "predictor.h"
#include "example_input.h"
#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

static uint16_t fft_length = FFT_LENGTH;

static int32_t wave_length = SAMPLE_LENGTH;
static float32_t wave_data[SAMPLE_LENGTH] = WAVE_DATA;

void test_dct(){

	float32_t dct_in[40] = {};
	for(int i =0; i<40; i++){
		if (i <20){
			dct_in[i] = i;
		}
		else{
			dct_in[i] = 39-i;
		}
	}
	float32_t dct_out[19];
	dct_ii(dct_in, dct_out, (uint16_t) MFCC_FILTERS);
}

void test_fft(){

	float32_t* fft_input = (float32_t*) AM_EXTERNAL_SRAM_START_ADDRESS;
	float32_t* fft_output = fft_input+FFT_LENGTH;
	//float32_t* spectrogram = fftOutput+FFT_LENGTH;
	printf("Something\n");

	float32_t spectrogram[FFT_LENGTH];
	q7_t output[FFT_LENGTH];

	float32_t hamming[FFT_LENGTH];
	for(int i = 0; i < fft_length; i++){
	  hamming[i] = 0.54 - 0.46*(arm_cos_f32(2*M_PI*i/(fft_length-1)));
	}

	wave_to_spectrogram(fft_input, fft_output, wave_data, wave_length, spectrogram, output, hamming, (uint16_t) FFT_LENGTH);
}

void test_dense_layer(){
	q15_t in[4] = {8192, 8192, 8192, 8192};
	q15_t wt[4] = {8192, 8192, 8192, 8192};
	q15_t bias[1] = {-1024};
	q15_t out[6];
	q15_t buffer[10];

	arm_fully_connected_q15(in, wt, 4, 1,
				DENSE_BIAS_LSHIFT, DENSE_OUT_RSHIFT, bias, out, buffer);
	arm_fully_connected_q15(in, wt, 4, 1,
					DENSE_BIAS_LSHIFT, DENSE_OUT_RSHIFT-1, bias, out+1, buffer);
	arm_fully_connected_q15(in, wt, 4, 1,
					DENSE_BIAS_LSHIFT, DENSE_OUT_RSHIFT-2, bias, out+2, buffer);
	arm_fully_connected_q15(in, wt, 4, 1,
					DENSE_BIAS_LSHIFT-1, DENSE_OUT_RSHIFT, bias, out+3, buffer);
	arm_fully_connected_q15(in, wt, 4, 1,
					DENSE_BIAS_LSHIFT-2, DENSE_OUT_RSHIFT, bias, out+4, buffer);
	arm_fully_connected_q15(in, wt, 4, 1,
					DENSE_BIAS_LSHIFT-3, DENSE_OUT_RSHIFT, bias, out+5, buffer);

	for(int i = 0; i < 6; i++){
		printf("prediction %d is %d\n", i, out[i]);
	}
}

void test_maxpool(){
	q15_t in[8] = {1,2,5,6,3,4,7,8};
		q15_t out[8];
		arm_maxpool_q15_HWC(in,
				4,
				2,
				1,
				1,
				0,
				1,
				4,
				2,
				out);
}

void test_cnn(){

	static q15_t conv1_wt[CONV1_IN_CH * CONV1_KER_WID * CONV1_KER_HGT * CONV1_OUT_CH] = CONV1_WT;
	static q15_t conv1_bias[CONV1_OUT_CH] = CONV1_BIAS;

	static q15_t conv2_wt[CONV2_IN_CH * CONV2_KER_WID * CONV2_KER_HGT * CONV2_OUT_CH] = CONV2_WT;
	static q15_t conv2_bias[CONV2_OUT_CH] = CONV2_BIAS;

	static q15_t dense_wt[DENSE_IN_DIM*DENSE_OUT_DIM] = DENSE_WT;
	static q15_t dense_bias[DENSE_OUT_DIM] = DENSE_BIAS;

	q15_t output[DENSE_OUT_DIM];

	// include the input

	q15_t input_data[CONV1_IN_WID*CONV1_IN_HGT*CONV1_IN_CH] = NN_INPUT_DATA2;

	float32_t* spectrogram = (float32_t*) AM_EXTERNAL_SRAM_START_ADDRESS;
	q15_t* bufferA = (q15_t*) (spectrogram + CONV1_IN_WID*CONV1_IN_HGT*CONV1_IN_CH);

	q15_t* output_data =  bufferA + 2*CONV2_IN_CH*CONV2_KER_HGT*CONV2_KER_WID;
	q15_t* pooled_output = output_data + CONV1_OUT_WID*CONV1_OUT_HGT*CONV1_OUT_CH;
	q15_t* conv2_output = pooled_output + POOL1_OUT_HGT*POOL1_OUT_WID*CONV1_OUT_CH;

	arm_convolve_HWC_q15_fast_nonsquare(input_data, CONV1_IN_HGT, CONV1_IN_WID, CONV1_IN_CH, conv1_wt,
									CONV1_OUT_CH, CONV1_KER_HGT, CONV1_KER_WID, CONV1_PADDING, CONV1_PADDING,
									CONV1_STRIDE, CONV1_STRIDE, conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT,
									output_data, CONV1_OUT_HGT, CONV1_OUT_WID, bufferA, NULL);

	arm_relu_q15(output_data, (uint16_t) CONV1_OUT_WID*CONV1_OUT_HGT*CONV1_OUT_CH);

	arm_maxpool_q15_HWC(output_data, CONV1_OUT_HGT, CONV1_OUT_WID, CONV1_OUT_CH,
						POOL1_KER_DIM, POOL1_PADDING, POOL1_STRIDE, POOL1_OUT_HGT,
						POOL1_OUT_WID, pooled_output);

	arm_convolve_HWC_q15_fast_nonsquare(pooled_output, CONV2_IN_HGT, CONV2_IN_WID, CONV2_IN_CH, conv2_wt,
										CONV2_OUT_CH, CONV2_KER_HGT, CONV2_KER_WID, CONV2_PADDING, CONV2_PADDING,
										CONV2_STRIDE, CONV2_STRIDE, conv2_bias, CONV2_BIAS_LSHIFT, CONV2_OUT_RSHIFT,
										conv2_output, CONV2_OUT_HGT, CONV2_OUT_WID, bufferA, NULL);

	arm_relu_q15(conv2_output, (uint16_t) CONV2_OUT_WID*CONV2_OUT_HGT*CONV2_OUT_CH);

	arm_fully_connected_q15(conv2_output, dense_wt, DENSE_IN_DIM, DENSE_OUT_DIM,
			DENSE_BIAS_LSHIFT, DENSE_OUT_RSHIFT, dense_bias, output, bufferA);

	arm_nn_activations_direct_q15(output, DENSE_OUT_DIM, 1, ARM_SIGMOID);

	printf("prediction is %d\n",output[0]);

}

void test_mfcc(){

	//expected output -3.38144,  2.26806,  1.10886,  1.41176,  0.89197,  0.77487,    1.10383,  0.91298,  0.8448 ,  1.10242,  0.45764,  0.61017,    0.98014,  0.67756,  0.63162,  0.7539 ,  0.92361,  0.65669,    1.02561
    static float32_t mfcc_input[256] = MFCC_INPUT_DATA;
    float32_t filterbanks[FILTERBANKS_LENGTH] = FILTERBANKS;

    float32_t mfcc_output[FILTERBANKS_LENGTH] = {};
    calculate_MFCC(mfcc_input, filterbanks, mfcc_output, (uint16_t) MFCC_FILTERS);

}

void test_cnn_buffers(){
/*
    q7_t* conv1_wt = (q7_t*) AM_EXTERNAL_SRAM_START_ADDRESS;
    if(true){
		q7_t c1w_weights[CONV1_WT_NUM] = CONV1_WT;
		memcpy(conv1_wt, c1w_weights, CONV1_WT_NUM);
    }

    q7_t* conv1_bias = conv1_wt + CONV1_WT_NUM;
    if(true){
		q7_t c1b_weights[CONV1_OUT_CH] = CONV1_BIAS;
		memcpy(conv1_bias, c1b_weights, CONV1_OUT_CH);
    }

    q7_t* conv2_wt = conv1_bias + CONV1_OUT_CH;
    if(true){
		q7_t c2w_weights[CONV2_WT_1_NUM] = CONV2_WT_1;
		memcpy(conv2_wt, c2w_weights, CONV2_WT_1_NUM);
    }
    if(true){
		q7_t c2w_weights[CONV2_WT_2_NUM] = CONV2_WT_2;
		memcpy(conv2_wt+CONV2_WT_1_NUM, c2w_weights, CONV2_WT_2_NUM);
    }
    if(true){
		q7_t c2w_weights[CONV2_WT_3_NUM] = CONV2_WT_3;
		memcpy(conv2_wt+2*CONV2_WT_1_NUM, c2w_weights, CONV2_WT_3_NUM);
	}

	q7_t* conv2_bias = conv2_wt + CONV2_WT_NUM;
	if(true){
		q7_t c2b_weights[CONV2_OUT_CH] = CONV2_BIAS;
		memcpy(conv2_bias, c2b_weights, CONV2_OUT_CH);
	}

	q7_t* dense_wt = conv2_bias + CONV2_OUT_CH;
	if(true){
		q7_t dwt[DENSE_IN_DIM*DENSE_OUT_DIM] = DENSE_WT;
		memcpy(dense_wt, dwt, DENSE_IN_DIM*DENSE_OUT_DIM);
	}

	q7_t* dense_bias =	dense_wt + DENSE_IN_DIM*DENSE_OUT_DIM;
	q7_t db[DENSE_OUT_DIM] = DENSE_BIAS;
	memcpy(dense_bias, db, DENSE_OUT_DIM);

	q7_t* mid_buffer = dense_bias + DENSE_OUT_DIM;
	*/
}

void test_main(){

	AudioMoth_initialise();

	AudioMoth_setupSWOForPrint();

	AudioMoth_enableExternalSRAM();

	test_dct();

	test_fft();

	test_dense_layer();

	test_maxpool();

	test_cnn();

}
