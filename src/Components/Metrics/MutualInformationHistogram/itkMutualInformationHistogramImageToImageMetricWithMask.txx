/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
	
		Copyright (c) Insight Software Consortium. All rights reserved.
		See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
		
			This software is distributed WITHOUT ANY WARRANTY; without even 
			the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
			PURPOSE.  See the above copyright notices for more information.
			
=========================================================================*/
#ifndef __itkMutualInformationHistogramImageToImageMetricWithMask_TXX__
#define __itkMutualInformationHistogramImageToImageMetricWithMask_TXX__

#include "itkMutualInformationHistogramImageToImageMetricWithMask.h"
#include "itkHistogram.h"

namespace itk
{
	
	
	/**
	 * ********************* Constructor ****************************
	 */

	template <class TFixedImage, class TMovingImage>
		MutualInformationHistogramImageToImageMetricWithMask<TFixedImage, TMovingImage>
		::MutualInformationHistogramImageToImageMetricWithMask()
  {
	} // end Constructor
		
	
	/**
	 * ********************* EvaluateMeasure ************************
	 */

  template <class TFixedImage, class TMovingImage>
		typename MutualInformationHistogramImageToImageMetricWithMask<TFixedImage, \
		TMovingImage>::MeasureType
		MutualInformationHistogramImageToImageMetricWithMask<TFixedImage, \
		TMovingImage>
		::EvaluateMeasure(HistogramType& histogram) const
  {
    MeasureType entropyX = NumericTraits<MeasureType>::Zero;
    MeasureType entropyY = NumericTraits<MeasureType>::Zero;
    MeasureType jointEntropy = NumericTraits<MeasureType>::Zero;
    HistogramFrequencyType totalFreq = histogram.GetTotalFrequency();
		
    for (unsigned int i = 0; i < this->m_HistogramSize[0]; i++)
    {
      HistogramFrequencyType freq = histogram.GetFrequency(i, 0);
      if (freq > 0)
      {
        entropyX += freq*log(freq);
      }
    }
		
    entropyX = -entropyX/static_cast<MeasureType>(totalFreq) + log(totalFreq);
		
    for (unsigned int i = 0; i < this->m_HistogramSize[1]; i++)
    {
      HistogramFrequencyType freq = histogram.GetFrequency(i, 1);
      if (freq > 0) 
      {
        entropyY += freq*log(freq);
      }
    }
    
    entropyY = -entropyY/static_cast<MeasureType>(totalFreq) + log(totalFreq);
		
    HistogramIteratorType it = histogram.Begin();
    HistogramIteratorType end = histogram.End();
    while (it != end) 
    {
      HistogramFrequencyType freq = it.GetFrequency();
      if (freq > 0)
      {
        jointEntropy += freq*log(freq);
      }
      ++it;
    }
		
    jointEntropy = -jointEntropy/static_cast<MeasureType>(totalFreq) +
      log(totalFreq);
		
    return entropyX + entropyY - jointEntropy;

  } // end EvaluateMeasure


} // end namespace itk

#endif // end itkMutualInformationHistogramImageToImageMetricWithMask_TXX__

