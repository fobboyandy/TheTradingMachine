#include <cmath>
#include <array>
#include <list>

class BollingerBands
{
public:
    BollingerBands(unsigned int period, double scale):
        period_(period),
        scale_(scale)
    {
        sum_ = 0;
    }
    
    
    ~BollingerBands(){}
    
    // indices for output datapoints for this indicator
    enum DataPointIndex
    {
        UPPER,
        MIDDLE,
        LOWER,
        SIZE
    };

    using array_type = std::array<DataPoint, SIZE>;

    array_type computeIndicatorPoint(const DataPoint& sample)
    {
        array_type result;
        
        window_.push_front(sample.value);
        sum_ += sample.value;
        
        // adjust sum and window_ size fixed
        if(window_.size() > period_)
        {
            sum_ -= window_.back();
            window_.pop_back();
        }
        
        auto avg = sum_ / window_.size();
        
        result[MIDDLE] = DataPoint{sample.time, avg};
        
        double offset = 0;
        // calculating stdDev
        for(const auto& val: window_)
        {
            offset += pow(val - avg, 2);
        }
        offset /= window_.size();
        offset = scale_ * std::sqrt(offset);
        
        result[LOWER] = DataPoint{sample.time, avg - offset};
        result[UPPER] = DataPoint{sample.time, avg + offset};
        
        return result;
        
    }
    array_type recomputeIndicatorPoint(const DataPoint& sample)
    {
        array_type result;
        
        //replace the front value and recompute
        sum_ -= window_.front();
        window_.front() = sample.value;
        
        
        auto avg = sum_ / window_.size();
        
        result[MIDDLE] = DataPoint{sample.time, avg};
        
        double offset = 0;
        // calculating stdDev
        for(const auto& val: window_)
        {
            offset += pow(val - avg, 2);
        }
        offset /= window_.size();
        offset = scale_ * std::sqrt(offset);
        
        result[LOWER] = DataPoint{sample.time, avg - offset};
        result[UPPER] = DataPoint{sample.time, avg + offset};
        
        return result;
    }
    
    private:
    const unsigned int period_;
    const double scale_;
    
    std::list<double> window_;
    double sum_;
    
};
