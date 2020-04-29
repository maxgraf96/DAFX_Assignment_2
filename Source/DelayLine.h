/*
  ==============================================================================

    DelayLine.h
    Created: 30 Mar 2020 2:26:11pm
    Author:  Music

  ==============================================================================
*/

#pragma once
template <typename Type>
class DelayLine
{
public:
	// Clear delay line (set all values to 0)
    void clear() noexcept
    {
        std::fill(rawData.begin(), rawData.end(), Type(0));
    }

	// Return the size of the delay line
    size_t size() const noexcept
    {
        return rawData.size();
    }

	// resize the delay line
    void resize(size_t newValue)
    {
        rawData.resize(newValue);
        leastRecentIndex = 0;
    }

    Type back() const noexcept
    {
        return rawData[leastRecentIndex];
    }

	// Get sample for delay time
    Type get(size_t delayInSamples) const noexcept
    {
        jassert(delayInSamples >= 0 && delayInSamples < size());

        return rawData[(leastRecentIndex + 1 + delayInSamples) % size()];
    }

    // Set the specified sample in the delay line
    void set(size_t delayInSamples, Type newValue) noexcept
    {
        jassert(delayInSamples >= 0 && delayInSamples < size());

        rawData[(leastRecentIndex + 1 + delayInSamples) % size()] = newValue;
    }

    // Adds a new value to the delay line, overwriting the least recently added sample
    void push(Type valueToAdd) noexcept
    {
        rawData[leastRecentIndex] = valueToAdd;
        leastRecentIndex = leastRecentIndex == 0 ? size() - 1 : leastRecentIndex - 1;
    }

private:
    std::vector<Type> rawData;
    size_t leastRecentIndex = 0;
};