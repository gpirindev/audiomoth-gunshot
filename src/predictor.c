#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "arm_const_structs.h"
#include "arm_math.h"
#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"
#include "predictor.h"
#include "weights.h"

static float32_t filterbanks[FILTERBANKS_LENGTH] = FILTERBANKS;

/* Compute the a spectrogram of a wave
 * with fftLen samples per time window */

void wave_to_spectrogram(float32_t *fft_input, float32_t *fft_output, q15_t *wave, int32_t wave_length, float32_t *spectrogram, q7_t* output, float32_t *hamming, uint16_t fft_len){

  arm_rfft_fast_instance_f32 rfftInstance;
  arm_status status = arm_rfft_fast_init_f32(&rfftInstance, fft_len);

  /* Variables for the FFT */
  uint32_t output_buffer_offset = 0;
  uint16_t features_per_sample = MFCC_FILTERS/2 -1;
  uint16_t sample_start = 0;
  uint16_t sample_end = fft_len;
  uint16_t step_size = fft_len/2;
  uint32_t freq_bins_num = fft_len/2;

  while(sample_end <= wave_length) {
	  /* Take the sample */

	  arm_q15_to_float(wave+sample_start, fft_input, fft_len);

	  /* Apply the Hamming window */
	  arm_mult_f32(fft_input, hamming, fft_input, (uint32_t) fft_len);

	  /* Compute the FFT.
	   * fft_input and fft_output should not overlap */

	  arm_rfft_fast_f32(&rfftInstance, fft_input, fft_output, 0);

	  arm_cmplx_mag_f32(fft_output, fft_input, freq_bins_num);

	  calculate_MFCC(fft_input, filterbanks, spectrogram + output_buffer_offset, (uint16_t) MFCC_FILTERS);

	  /* Update the FFT time window */
	  output_buffer_offset += features_per_sample;
	  sample_start += step_size;
	  sample_end += step_size;


  }

  float32_t max_elem, min_elem;
  uint32_t max_elem_index, min_elem_index;
  arm_max_f32(spectrogram, output_buffer_offset, &max_elem, &max_elem_index);
  arm_min_f32(spectrogram, output_buffer_offset, &min_elem, &min_elem_index);

  float32_t abs_max = 1/min_elem;
  if(max_elem*max_elem >= min_elem*min_elem){
	  abs_max = 1/max_elem;
  }

  arm_scale_f32(spectrogram, abs_max, spectrogram, output_buffer_offset);

  arm_float_to_q7(spectrogram, output, output_buffer_offset);
}

void forward_propagate_q7_cnn(q7_t *input ,q7_t *mid_buffer, q7_t *output, const q7_t *conv1_wt, const q7_t *conv1_bias,
		const q7_t *conv2_wt, const q7_t *conv2_bias, const q7_t *dense_l1_wt, const q7_t *dense_l1_bias, const q7_t *dense_l2_wt, const q7_t *dense_l2_bias){

	//helper_buffer will be used as a buffer to store the imcol in the convolutions
	//buffer1 and buffer2 will contain intermediate results
	q7_t* buffer1 = mid_buffer;
	//buffer1 must have length MAX(conv1 output, conv2 output)
	//buffer2 must have length len(maxpool output)
	q7_t* buffer2 = buffer1 + CNN_BUFFER_1_SIZE;
	q15_t* helper_buffer = (q15_t*) (buffer2 + CNN_BUFFER_2_SIZE);

	// conv1 input_data -> buffer1
	arm_convolve_HWC_q7_basic_nonsquare(input, CONV1_IN_HGT, CONV1_IN_WID, CONV1_IN_CH, conv1_wt,
									CONV1_OUT_CH, CONV1_KER_HGT, CONV1_KER_WID, CONV1_PADDING, CONV1_PADDING,
									CONV1_STRIDE, CONV1_STRIDE, conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT,
									buffer1, CONV1_OUT_HGT, CONV1_OUT_WID, helper_buffer, NULL);

	arm_relu_q7(buffer1, (uint16_t) CONV1_OUT_WID*CONV1_OUT_HGT*CONV1_OUT_CH);

	// maxpool buffer1 -> buffer2
	arm_maxpool_q7_HWC_nonsquare(buffer1, CONV1_OUT_HGT, CONV1_OUT_WID, CONV1_OUT_CH,
						POOL1_KER_DIM, POOL1_PADDING, POOL1_STRIDE, POOL1_OUT_HGT,
						POOL1_OUT_WID, buffer2);

	// conv2 buffer2 -> buffer1
	arm_convolve_HWC_q7_fast_nonsquare(buffer2, CONV2_IN_HGT, CONV2_IN_WID, CONV2_IN_CH, conv2_wt,
										CONV2_OUT_CH, CONV2_KER_HGT, CONV2_KER_WID, CONV2_PADDING, CONV2_PADDING,
										CONV2_STRIDE, CONV2_STRIDE, conv2_bias, CONV2_BIAS_LSHIFT, CONV2_OUT_RSHIFT,
										buffer1, CONV2_OUT_HGT, CONV2_OUT_WID, helper_buffer, NULL);

	arm_relu_q7(buffer1, (uint16_t) CONV2_OUT_WID*CONV2_OUT_HGT*CONV2_OUT_CH);

	// dense buffer1 -> output
	arm_fully_connected_q7(buffer1, dense_l1_wt, DENSE_L1_IN_DIM, DENSE_L1_OUT_DIM,
			DENSE_L1_BIAS_LSHIFT, DENSE_L1_OUT_RSHIFT, dense_l1_bias, buffer2, helper_buffer);

	arm_relu_q7(buffer2, (uint16_t) DENSE_L1_OUT_DIM);

	arm_fully_connected_q7(buffer2, dense_l2_wt, DENSE_L2_IN_DIM, DENSE_L2_OUT_DIM,
			DENSE_L2_BIAS_LSHIFT, DENSE_L2_OUT_RSHIFT, dense_l2_bias, output, helper_buffer);

	arm_nn_activations_direct_q7(output, DENSE_L2_OUT_DIM, 1, ARM_SIGMOID);

}

void forward_propagate_q7_dnn(q7_t *input ,q7_t *mid_buffer, q7_t *output, const q7_t *dense1_wt, const q7_t *dense1_bias,
		const q7_t *dense2_wt, const q7_t *dense2_bias, const q7_t *dense3_wt, const q7_t *dense3_bias,
		const q7_t *dense4_wt, const q7_t *dense4_bias){

	//helper_buffer will be used as a buffer to store the intermediate computations
	//buffer1 and buffer2 will contain intermediate results
	q7_t* buffer1 = mid_buffer;
	//buffer1 and buffer2 must have length len(dense_dim output)
	q7_t* buffer2 = buffer1 + DNN_BUFFER_SIZE;
	q15_t* helper_buffer = (q15_t*) (buffer2 + DNN_BUFFER_SIZE);

	arm_fully_connected_q7(input, dense1_wt, DENSE1_IN_DIM, DENSE1_OUT_DIM,
				DENSE1_BIAS_LSHIFT, DENSE1_OUT_RSHIFT, dense1_bias, buffer1, helper_buffer);


	arm_relu_q7(buffer1, (uint16_t) DENSE1_OUT_DIM);

	arm_fully_connected_q7(buffer1, dense2_wt, DENSE2_IN_DIM, DENSE2_OUT_DIM,
				DENSE2_BIAS_LSHIFT, DENSE2_OUT_RSHIFT, dense2_bias, buffer2, helper_buffer);


	arm_relu_q7(buffer2, (uint16_t) DENSE2_OUT_DIM);

	arm_fully_connected_q7(buffer2, dense3_wt, DENSE3_IN_DIM, DENSE3_OUT_DIM,
				DENSE3_BIAS_LSHIFT, DENSE3_OUT_RSHIFT, dense3_bias, buffer1, helper_buffer);


	arm_relu_q7(buffer1, (uint16_t) DENSE3_OUT_DIM);

	arm_fully_connected_q7(buffer1, dense4_wt, DENSE4_IN_DIM, DENSE4_OUT_DIM,
				DENSE4_BIAS_LSHIFT, DENSE4_OUT_RSHIFT, dense4_bias, output, helper_buffer);

	arm_nn_activations_direct_q7(output, DENSE4_OUT_DIM, 1, ARM_SIGMOID);

}

void arm_maxpool_q15_HWC(q15_t * Im_in,
		const uint16_t dim_in_h,
		const uint16_t dim_in_w,
		const uint16_t ch_im_in,
		const uint16_t dim_kernel,
		const uint16_t padding,
		const uint16_t stride,
		const uint16_t dim_out_h,
		const uint16_t dim_out_w,
		q15_t * Im_out)
{
	int16_t   i_ch_in, i_x, i_y;
	int16_t   k_x, k_y;

	for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
	{
		for (i_y = 0; i_y < dim_out_w; i_y++)
		{
			for (i_x = 0; i_x < dim_out_h; i_x++)
			{
				int       max = -32769; // i.e., -1 * (2^15+1)
				for (k_y = i_y * stride - padding; k_y < i_y * stride - padding + dim_kernel; k_y++)
				{
					for (k_x = i_x * stride - padding; k_x < i_x * stride - padding + dim_kernel; k_x++)
					{
						if (k_y >= 0 && k_x >= 0 && k_y < dim_in_w && k_x < dim_in_h)
						{
							if (Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_in_h)] > max)
							{
								max = Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_in_h)];
							}
						}
					}
				}
				Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_out_h)] = max;
			}
		}
	}
}

void arm_maxpool_q7_HWC_nonsquare(q7_t * Im_in,
		const uint16_t dim_in_h,
		const uint16_t dim_in_w,
		const uint16_t ch_im_in,
		const uint16_t dim_kernel,
		const uint16_t padding,
		const uint16_t stride,
		const uint16_t dim_out_h,
		const uint16_t dim_out_w,
		q7_t * Im_out)
{
	int16_t   i_ch_in, i_x, i_y;
	int16_t   k_x, k_y;

	for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
	{
		for (i_y = 0; i_y < dim_out_w; i_y++)
		{
			for (i_x = 0; i_x < dim_out_h; i_x++)
			{
				int16_t       max = -129; // i.e., -1 * (2^7+1)
				for (k_y = i_y * stride - padding; k_y < i_y * stride - padding + dim_kernel; k_y++)
				{
					for (k_x = i_x * stride - padding; k_x < i_x * stride - padding + dim_kernel; k_x++)
					{
						if (k_y >= 0 && k_x >= 0 && k_y < dim_in_w && k_x < dim_in_h)
						{
							if (Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_in_h)] > max)
							{
								max = Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_in_h)];
							}
						}
					}
				}
				Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_out_h)] = max;
			}
		}
	}
}

void dct_ii(float32_t* dct_input, float32_t* dct_output, const uint16_t length){
	float32_t X_k = 0;
	float32_t factor;
	for(uint16_t k = 1; k<length/2; k++){
		X_k = 0;
		factor = k*M_PI/length;
		for(uint16_t i = 0; i<length; i++){
			X_k += dct_input[i]*arm_cos_f32((i+0.5f)*factor);
		}
		dct_output[k-1] = X_k*sqrt(2.0f/(float32_t) length);
	}
}

/* filterbanks is an array that has the following shape:
 * for each filter [s,e,f1,f2,f3,...]
 * s gives the start index for the fft bin
 * e gives the end index for the fft bin
 * f1,f2,f3,.. are the filter values between s and e
*/
void calculate_MFCC(float32_t* mfcc_input, float32_t* filterbanks, float32_t* mfcc_output, const uint16_t number_of_filters){
	// the filterbanks array index
	uint16_t index = 0;
	float32_t energy = 0.0f;
	uint16_t start_index;
	uint16_t end_index;
	float32_t buffer[number_of_filters];

	for(uint16_t i =0; i< number_of_filters; i++){
		start_index = (uint16_t) filterbanks[index++];
		end_index = (uint16_t) filterbanks[index++];
		for(uint16_t j = start_index; j< end_index+1; j++){
			energy += mfcc_input[j]*filterbanks[index++];
		}
		if(energy == 0){
			energy = 1e-6;
		}
		buffer[i] = log(energy);
		energy = 0;
	}

	dct_ii(buffer, mfcc_output, number_of_filters);
}
