/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utilities.hpp"
#include "soundfile.hpp"
#define PCM_READ_SIZE 2048
#include <math.h>
#include <vector>
#include <map>
#include <xtract/libxtract.h>
//harmonicity takes a long time, you can skip windows by making this larger than 0
#define SKIP_HARM_WINDOWS 1

#include <sstream>

void DataJockey::soundFileToWave(std::string inFileName, std::string outFileName){
	SoundFile inFile(inFileName.c_str());
	short inbuf[PCM_READ_SIZE];
	unsigned int frames_read;

	//check to make sure soundfile exists
	if(!inFile){
		std::string str("cannot open input soundfile: ");
		str.append(inFileName);
		throw std::runtime_error(str);
	}

	//we open the output file, we want it to be a wave file
	SndfileHandle outFile(outFileName.c_str(), 
			SFM_WRITE, 
			SF_FORMAT_WAV | SF_FORMAT_PCM_16, 
			inFile.channels(), 
			inFile.samplerate());
	if(!outFile){
		std::string str("cannot open output soundfile: ");
		str.append(outFileName);
		throw std::runtime_error(str);
	}

	//read in the audio data and write to the output file
	while((frames_read = inFile.readf(inbuf, PCM_READ_SIZE / inFile.channels())) != 0)
		outFile.writef(inbuf, frames_read);
}

float median(std::vector<float> &inVec){
	std::sort(inVec.begin(), inVec.end());
	if(inVec.size() % 2)
		return inVec[inVec.size() / 2];
	else
		return (inVec[inVec.size() / 2] + inVec[inVec.size() / 2 + 1]) / 2.0;
}

std::string DataJockey::computeDescriptors(std::string inFileName, unsigned int windowSize){
	float *window = new float[windowSize];
	float *monoBuf = new float[windowSize];
	float *monoBufWindowed = new float[windowSize];
	float *doubleMonoBuf = new float[windowSize * 2];
	float inbuf[PCM_READ_SIZE];
	unsigned int frames_read;
	unsigned int channels;

	std::vector<float> audioData;
	std::map<std::string, std::vector<float> * > descriptors;
	std::vector<float> centroidList;
	std::vector<float> irregularityJList;
	std::vector<float> irregularityKList;
	//std::vector<float> spectralMeanList;
	//std::vector<float> spectralVarianceList;
	//std::vector<float> f0List;
	std::vector<float> spectralSlopeList;
	std::vector<float> spectralSpreadList;
	std::vector<float> harmonicityList;

	SoundFile soundFile(inFileName.c_str());
	//check to make sure soundfile exists
	if(!soundFile){
		std::string str("cannot open input soundfile: ");
		str.append(inFileName);
		throw std::runtime_error(str);
	}

	channels = soundFile.channels();

	//read the sound file into memory and make it mono
	//read in the audio data and write to the output file
	while((frames_read = soundFile.readf(inbuf, PCM_READ_SIZE / channels)) != 0){
		for(unsigned int i = 0; i < frames_read; i++){
			float sum = 0;
			for(unsigned int j = 0; j < channels; j++)
				sum += inbuf[i * channels + j];
			audioData.push_back(sum / channels);
		}
	}

	//generate the window [triangle]
	window[windowSize / 2] = 1.0;
	for(unsigned int i = 0; i < windowSize / 2; i++)
		window[windowSize - i - 1] = window[i] = ((float) i) * 2 / windowSize;

	//xtract_init_fft(windowSize, XTRACT_SPECTRUM);

	for(unsigned int i = 0; ((i + 1) * windowSize) < audioData.size(); i += 1){
		float argv[3];

		//window the b.s. and copy it to the fftInBuf
		for(unsigned int j = 0; j < windowSize; j++){
			//mix the 2 channels [we just care about mono]
			
			//doubleMonoBuf[j + (i % 2) * windowSize] = 
				//monoBuf[j] = (audioInBuf[j * 2] + audioInBuf[j * 2 + 1]) / 2.0;
			doubleMonoBuf[j + (i % 2) * windowSize] = 
				monoBuf[j] = audioData[j + i * windowSize];

			monoBufWindowed[j] = window[j] * monoBuf[j];
		}

		//compute spectrum
		float spectrum[windowSize];
		argv[0] = soundFile.samplerate() / (float)windowSize;
		argv[1] = XTRACT_MAGNITUDE_SPECTRUM;
		argv[2] = 0;
		xtract[XTRACT_SPECTRUM](monoBufWindowed, windowSize, argv, spectrum);

//		float peakSpectrum[windowSize + 1];
//		//compute peak spectrum
//		argv[0] = soundFile->samplerate() / ((float)windowSize / 2);
//		//XXX what should this value be? 
//		argv[1] = 0.1;
//		xtract[XTRACT_PEAK_SPECTRUM](spectrum, windowSize / 2, argv, peakSpectrum);

		//compute spectral centroid
		float centroid;
		xtract[XTRACT_SPECTRAL_CENTROID](spectrum, windowSize, NULL, &centroid);
		if(isnan(centroid) || isinf(centroid)){
			//cout << "centroid (nan/inf): " << float(i) / soundFile->samplerate() << std::endl;
			centroid = 0;
		}
		centroidList.push_back(centroid);
		//vector_extensions::insert_sorted(centroidList, centroid);

		////compute spectralMean
		//float spectralMean;
		//xtract[XTRACT_SPECTRAL_MEAN](spectrum, windowSize, NULL, &spectralMean);
		//if(isnan(spectralMean) || isinf(spectralMean)){
			////cout << "spectralMean (nan/inf): " << float(i) / soundFile->samplerate() << std::endl;
			//spectralMean = 0;
		//}
		//vector_extensions::insert_sorted(spectralMeanList, spectralMean);

		////compute spectralVariance
		//float spectralVariance;
		//xtract[XTRACT_SPECTRAL_VARIANCE](spectrum, windowSize, &spectralMean, &spectralVariance);
		//if(isnan(spectralVariance) || isinf(spectralVariance)){
			////cout << "spectralVariance (nan/inf): " << float(i) / soundFile->samplerate() << std::endl;
			//spectralVariance = 0;
		//}
		//vector_extensions::insert_sorted(spectralVarianceList, spectralVariance);

		//compute irregularityJ
		float irregularityJ;
		xtract[XTRACT_IRREGULARITY_J](spectrum, windowSize/ 2, NULL, &irregularityJ);
		if(isnan(irregularityJ) || isinf(irregularityJ))
			irregularityJ = 0;
		irregularityJList.push_back(irregularityJ);
		//vector_extensions::insert_sorted(irregularityJList, irregularityJ);
		
		//compute irregularityK
		float irregularityK;
		xtract[XTRACT_IRREGULARITY_K](spectrum, windowSize/ 2, NULL, &irregularityK);
		if(isnan(irregularityK) || isinf(irregularityK))
			irregularityK = 0;
		//vector_extensions::insert_sorted(irregularityKList, irregularityK);
		irregularityKList.push_back(irregularityK);

		/*
		//compute f0
		float f0;
		argv[0] = soundFile->samplerate();
		xtract[XTRACT_FAILSAFE_F0](monoBuf, windowSize, argv, &f0);
		vector_extensions::insert_sorted(f0List, f0);
		*/
		
		//compute spectralSlope
		float spectralSlope;
		xtract[XTRACT_SPECTRAL_SLOPE](spectrum, windowSize, NULL, &spectralSlope);
		if(isnan(spectralSlope) || isinf(spectralSlope))
			spectralSlope = 0;
		//vector_extensions::insert_sorted(spectralSlopeList, spectralSlope);
		spectralSlopeList.push_back(spectralSlope);


		//compute spectralSpread
		float spectralSpread;
		xtract[XTRACT_SPREAD](spectrum, windowSize/2, &centroid, &spectralSpread);
		if(isnan(spectralSpread) || isinf(spectralSpread))
			spectralSpread = 0;
		//vector_extensions::insert_sorted(spectralSpreadList, spectralSpread);
		spectralSpreadList.push_back(spectralSpread);

		if((i % (2 * (1 + SKIP_HARM_WINDOWS))) == 1){
			//compute the harmonicity
			//file:///home/alex/doc/harmonicity-email-easy.html
			//RB(k) = SUM (x(n) * x(n+k))
			//H11 = MAX (RB(k)) / RB(0)
			//where the maximum is taken over k in the range [1, MAX], where MAX should
			//be larger than any likely period of the harmonic component.
			double sum;
			double max_sum = -1000000000000000.0f;
			for(unsigned int k = 1; k < windowSize; k++){
				sum = 0;
				for(unsigned int n = 0; n < windowSize; n++){
					sum += doubleMonoBuf[n] * doubleMonoBuf[n + k];
				}
				if(sum > max_sum)
					max_sum = sum;
			}
			sum = 0;
			for(unsigned int n = 0; n < windowSize; n++){
				sum += pow(doubleMonoBuf[n], 2);
			}
			if (sum > 0){
				if (max_sum < sum){
					harmonicityList.push_back((float)(max_sum / sum));
					//vector_extensions::insert_sorted(harmonicityList, (float)(max_sum / sum));
				}
			}
		}
	}

	////set the precision
	//cout.setf(0,ios::floatfield);
	std::ostringstream outStringStream;
	outStringStream.width(std::numeric_limits< float >::digits10);

	//output descriptor name\twindow size\thop size
	//values0 value1
	//average median
	
	//output the values and compute the average and median
	outStringStream << "spectral centroid\t" << windowSize << "\t" << windowSize << std::endl;
	double sum = 0;
	for(std::vector<float>::iterator it = centroidList.begin(); it != centroidList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / centroidList.size() << "\t" << median(centroidList) << std::endl;

	sum = 0;
	outStringStream << "harmonicity\t" << windowSize << "\t" << windowSize  * (1 + SKIP_HARM_WINDOWS) << std::endl;
	for(std::vector<float>::iterator it = harmonicityList.begin(); it != harmonicityList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / harmonicityList.size() << "\t" << median(harmonicityList) << std::endl;

	sum = 0;
	outStringStream << "spectral irregularityJ\t" << windowSize << "\t" << windowSize << std::endl;
	for(std::vector<float>::iterator it = irregularityJList.begin(); it != irregularityJList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / irregularityJList.size() << "\t" << median(irregularityJList) << std::endl;

	sum = 0;
	outStringStream << "spectral irregularityK\t" << windowSize << "\t" << windowSize << std::endl;
	for(std::vector<float>::iterator it = irregularityKList.begin(); it != irregularityKList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / irregularityKList.size() << "\t" << median(irregularityKList) << std::endl;

	sum = 0;
	outStringStream << "spectral slope\t" << windowSize << "\t" << windowSize << std::endl;
	for(std::vector<float>::iterator it = spectralSlopeList.begin(); it != spectralSlopeList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / spectralSlopeList.size() << "\t" << median(spectralSlopeList) << std::endl;
		
	sum = 0;
	outStringStream << "spectral spread\t" << windowSize << "\t" << windowSize << std::endl;
	for(std::vector<float>::iterator it = spectralSpreadList.begin(); it != spectralSpreadList.end(); it++){
		outStringStream << *it << "\t";
		sum += *it;
	}
	outStringStream << std::endl;
	outStringStream << sum / spectralSpreadList.size() << "\t" << median(spectralSpreadList) << std::endl;

	return outStringStream.str();
}

