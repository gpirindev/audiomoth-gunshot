#ifndef PREDICTOR_H
#define PREDICTOR_H

void wave_to_spectrogram(float32_t *fftInput, float32_t *fftOutput, q15_t *wave, int32_t waveLength, float32_t *spectrogram, q7_t* output, float32_t *hamming, uint16_t fftLen);
void forward_propagate_q7_cnn(q7_t *input ,q7_t *mid_buffer, q7_t *output, const q7_t *conv1_wt, const q7_t *conv1_bias,
		const q7_t *conv2_wt, const q7_t *conv2_bias, const q7_t *dense_l1_wt, const q7_t *dense_l1_bias, const q7_t *dense_l2_wt, const q7_t *dense_l2_bias);
void forward_propagate_q7_dnn(q7_t *input ,q7_t *mid_buffer, q7_t *output, const q7_t *dense1_wt, const q7_t *dense1_bias,
		const q7_t *dense2_wt, const q7_t *dense2_bias, const q7_t *dense3_wt, const q7_t *dense3_bias,
		const q7_t *dense4_wt, const q7_t *dense4_bias);
void arm_maxpool_q15_HWC(q15_t * Im_in, const uint16_t dim_in_h, const uint16_t dim_in_w, const uint16_t ch_im_in, const uint16_t dim_kernel, const uint16_t padding, const uint16_t stride, const uint16_t dim_out_h, const uint16_t dim_out_w, q15_t * Im_out);
void arm_maxpool_q7_HWC_nonsquare(q7_t * Im_in,
		const uint16_t dim_in_h,
		const uint16_t dim_in_w,
		const uint16_t ch_im_in,
		const uint16_t dim_kernel,
		const uint16_t padding,
		const uint16_t stride,
		const uint16_t dim_out_h,
		const uint16_t dim_out_w,
		q7_t * Im_out);
void calculate_MFCC(float32_t* mfcc_input, float32_t* filterbanks, float32_t* mfcc_output, uint16_t number_of_filters);
void dct_ii(float32_t* dct_input, float32_t* dct_output, uint16_t length);
#endif
