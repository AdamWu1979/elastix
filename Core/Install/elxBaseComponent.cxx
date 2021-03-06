/*=========================================================================
 *
 *  Copyright UMC Utrecht and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "elxBaseComponent.h"

#include "itkNumberToString.h"

#include <cassert>
#include <cmath>   // For fmod.
#include <iomanip> // For setprecision.
#include <numeric> // For accumulate.
#include <regex>
#include <sstream> // For ostringstream.

namespace
{
bool
IsElastixLibrary(const bool initialValue = true)
{
  // By default, assume that this is the elastix library (not the elastix executable).

  // Note that the initialization of this static variable is thread-safe,
  // as supported by C++11 "magic statics".
  static const bool isElastixLibrary{ initialValue };

  return isElastixLibrary;
}
} // namespace

namespace elastix
{

/**
 * ****************** elxGetClassName ****************************
 */

const char *
BaseComponent::elxGetClassName(void) const
{
  return "BaseComponent";
} // end elxGetClassName()


/**
 * ****************** SetComponentLabel ****************************
 */

void
BaseComponent::SetComponentLabel(const char * label, unsigned int idx)
{
  std::ostringstream makestring;
  makestring << label << idx;
  this->m_ComponentLabel = makestring.str();
} // end SetComponentLabel()


/**
 * ****************** GetComponentLabel ****************************
 */

const char *
BaseComponent::GetComponentLabel(void) const
{
  return this->m_ComponentLabel.c_str();
} // end GetComponentLabel()


bool
BaseComponent::IsElastixLibrary()
{
  return ::IsElastixLibrary();
}

void
BaseComponent::InitializeElastixExecutable()
{
  ::IsElastixLibrary(false);
}

/**
 * ****************** ConvertSecondsToDHMS ****************************
 */

std::string
BaseComponent::ConvertSecondsToDHMS(const double totalSeconds, const unsigned int precision)
{
  /** Define days, hours, minutes. */
  const std::size_t secondsPerMinute = 60;
  const std::size_t secondsPerHour = 60 * secondsPerMinute;
  const std::size_t secondsPerDay = 24 * secondsPerHour;

  /** Convert total seconds. */
  std::size_t       iSeconds = static_cast<std::size_t>(totalSeconds);
  const std::size_t days = iSeconds / secondsPerDay;

  iSeconds %= secondsPerDay;
  const std::size_t hours = iSeconds / secondsPerHour;

  iSeconds %= secondsPerHour;
  const std::size_t minutes = iSeconds / secondsPerMinute;

  // iSeconds %= secondsPerMinute;
  // const std::size_t seconds = iSeconds;
  const double dSeconds = fmod(totalSeconds, 60.0);

  /** Create a string in days, hours, minutes and seconds. */
  bool               nonzero = false;
  std::ostringstream make_string("");
  if (days != 0)
  {
    make_string << days << "d";
    nonzero = true;
  }
  if (hours != 0 || nonzero)
  {
    make_string << hours << "h";
    nonzero = true;
  }
  if (minutes != 0 || nonzero)
  {
    make_string << minutes << "m";
  }
  make_string << std::showpoint << std::fixed << std::setprecision(precision);
  make_string << dSeconds << "s";

  /** Return a value. */
  return make_string.str();

} // end ConvertSecondsToDHMS()


/**
 * ****************** ToString ****************************
 */

std::string
BaseComponent::ParameterMapToString(const ParameterMapType & parameterMap)
{
  const auto expectedNumberOfChars = std::accumulate(
    parameterMap.cbegin(),
    parameterMap.cend(),
    std::size_t{},
    [](const std::size_t numberOfChars, const std::pair<std::string, ParameterValuesType> & parameter) {
      return numberOfChars +
             std::accumulate(parameter.second.cbegin(),
                             parameter.second.cend(),
                             // Two parentheses and a linebreak are added for each parameter.
                             parameter.first.size() + 3,
                             [](const std::size_t numberOfCharsPerParameter, const std::string & value) {
                               // A space character is added for each of the values.
                               // Plus two double-quotes, if the value is not a number.
                               return numberOfCharsPerParameter + value.size() +
                                      (BaseComponent::IsNumber(value) ? 1 : 3);
                             });
    });

  std::string result;
  result.reserve(expectedNumberOfChars);

  for (const auto & parameter : parameterMap)
  {
    result.push_back('(');
    result.append(parameter.first);

    for (const auto & value : parameter.second)
    {
      result.push_back(' ');

      if (BaseComponent::IsNumber(value))
      {
        result.append(value);
      }
      else
      {
        result.push_back('"');
        result.append(value);
        result.push_back('"');
      }
    }
    result.append(")\n");
  }

  // Assert that the correct number of characters was reserved.
  assert(result.size() == expectedNumberOfChars);
  return result;
}


std::string
BaseComponent::ToString(const double scalar)
{
  return itk::NumberToString<double>{}(scalar);
}

bool
BaseComponent::IsNumber(const std::string & str)
{
  auto       iter = str.cbegin();
  const auto end = str.cend();

  if (iter == end)
  {
    return false;
  }
  if (*iter == '-')
  {
    // Skip minus sign.
    ++iter;

    if (iter == end)
    {
      return false;
    }
  }

  const auto isDigit = [](const char ch) { return (ch >= '0') && (ch <= '9'); };

  if (!(isDigit(*iter) && isDigit(str.back())))
  {
    // Any number must start and end with a digit.
    return false;
  }
  ++iter;

  const auto numberOfChars = end - iter;
  const auto numberOfDigits = std::count_if(iter, end, isDigit);

  if (numberOfDigits == numberOfChars)
  {
    // Whole (integral) number, e.g.: 1234567890
    return true;
  }

  if ((std::find(iter, end, '.') != end) && (numberOfDigits == (numberOfChars - 1)))
  {
    // Decimal notation, e.g.: 12345.67890
    return true;
  }
  // Scientific notation, e.g.: -1.23e-89 (Note: `iter` has already parsed the optional minus sign and the first digit.
  return std::regex_match(iter, end, std::regex("(\\.\\d+)?e[+-]\\d+"));
}


} // end namespace elastix
