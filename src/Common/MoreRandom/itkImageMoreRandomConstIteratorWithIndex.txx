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
#ifndef _itkImageMoreRandomConstIteratorWithIndex_txx
#define _itkImageMoreRandomConstIteratorWithIndex_txx

#include "itkImageMoreRandomConstIteratorWithIndex.h"

//#include "vnl/vnl_sample.h"

//more random, do not forget to link with vnl_random.cxx:
#include "elx_sample.h"

namespace itk
{


/** Default constructor. Needed since we provide a cast constructor. */
template<class TImage>
ImageMoreRandomConstIteratorWithIndex<TImage>
::ImageMoreRandomConstIteratorWithIndex() : ImageConstIteratorWithIndex<TImage>()
{
  m_NumberOfPixelsInRegion    = 0L;
  m_NumberOfSamplesRequested  = 0L;
  m_NumberOfSamplesDone       = 0L;
  m_NumberOfRandomJumps       = 0L;
}

/** Constructor establishes an iterator to walk a particular image and a
 * particular region of that image. */
template<class TImage>
ImageMoreRandomConstIteratorWithIndex<TImage>
::ImageMoreRandomConstIteratorWithIndex(const ImageType *ptr, const RegionType& region)
  : ImageConstIteratorWithIndex<TImage>( ptr, region )
{
  m_NumberOfPixelsInRegion   = region.GetNumberOfPixels();
  m_NumberOfSamplesRequested = 0L;
  m_NumberOfSamplesDone      = 0L;
}

/**  Set the number of samples to extract from the region */
template<class TImage>
void
ImageMoreRandomConstIteratorWithIndex<TImage>
::SetNumberOfSamples( unsigned long number )
{
  m_NumberOfSamplesRequested = number;
}

/**  Set the number of samples to extract from the region */
template<class TImage>
unsigned long
ImageMoreRandomConstIteratorWithIndex<TImage>
::GetNumberOfSamples( void ) const
{
  return m_NumberOfSamplesRequested;
}

/** Reinitialize the seed of the random number generator */
template<class TImage>
void
ImageMoreRandomConstIteratorWithIndex<TImage>
::ReinitializeSeed()
{
  elx_sample_reseed();
}

template<class TImage>
void
ImageMoreRandomConstIteratorWithIndex<TImage>
::ReinitializeSeed(int seed)
{
  elx_sample_reseed(seed);
}

/** Execute an acrobatic random jump */
template<class TImage>
void
ImageMoreRandomConstIteratorWithIndex<TImage>
::RandomJump()
{
  const unsigned long randomPosition =
    static_cast<unsigned long > (
      elx_sample_uniform(0.0f, 
                         static_cast<double>(m_NumberOfPixelsInRegion)-0.5) );
  
  unsigned long position = randomPosition;
  unsigned long residual;
  for( unsigned int dim = 0; dim < TImage::ImageDimension; dim++ )
    {
    const unsigned long sizeInThisDimension = m_Region.GetSize()[dim];
    residual = position % sizeInThisDimension;
    m_PositionIndex[dim] =  residual + m_BeginIndex[dim];
    position -= residual;
    position /= sizeInThisDimension;
    }

  m_Position = m_Image->GetBufferPointer() + m_Image->ComputeOffset( m_PositionIndex );
}


} // end namespace itk

#endif
