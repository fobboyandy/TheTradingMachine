class RelativeStrengthIndex
{
public:
    
    enum
    {
        RSI,
        SIZE
    };
    
    RelativeStrengthIndex(unsigned int period):
        period_(period)
    {
        size_ = 0;
        pVal_ = cVal_ = 0;
        pSUp_ = cSUp_ = 0;
        pSDown_ = cSDown_ = 0;
    }
    ~RelativeStrengthIndex(){}
    
    using array_type = std::array<DataPoint, SIZE>;
    array_type computeIndicatorPoint(const DataPoint& sample)
    {
        array_type result;
        
        // last modified values get stored into the p values;
        pVal_ = cVal_;
        pSUp_ = cSUp_;
        pSDown_ = cSDown_;
     
        // account for number of inputs for when num of inputs
        // hasn't reached period_, we output results using current 
        // size as the period
        if(size_ < period_)
            ++size_;
        
        double up, down;
        sample.value > pVal_ ? up = sample.value - pVal_ : up = 0;
        sample.value < pVal_ ? down = pVal_ - sample.value : down = 0;
        
        //cVal is now the newest value.
        cVal_ = sample.value;
        
        // calculate the newest current sup and sdown values
        cSUp_ = (size_ - 1) * pSUp_ / size_ + up / size_;
        cSDown_ = (size_ - 1) * pSDown_ / size_ + down / size_;
        
        return array_type{DataPoint{sample.time, 100 - 100 / (1 + cSUp_ / cSDown_)}};
    }
    array_type recomputeIndicatorPoint(const DataPoint& sample)
    {
        array_type result;
        
        double up, down;
        
        // compare with pVal_ to assign up and down. we compute relative
        // to the previous input
        sample.value > pVal_ ? up = sample.value - pVal_ : up = 0;
        sample.value < pVal_ ? down = pVal_ - sample.value : down = 0;
       
        //cVal is now the newest value.
        cVal_ = sample.value;
        
        // calculate the current sup and sdown values and store them.
        // they get transferred to p sup and sdown when new values in come
        cSUp_ = (size_ - 1) * pSUp_ / size_ + up / size_;
        cSDown_ = (size_ - 1) * pSDown_ / size_ + down / size_;
        
        return array_type{DataPoint{sample.time, 100 - 100 / (1 + cSUp_ / cSDown_)}};
    }
    
private:    
    const unsigned int period_;
    double pVal_;
    
    // previous s up and s down. gets replaced by current s up and down for every new value (candle)
    // in computeIndicatorPoint
    double pSUp_;
    double pSDown_;
    
    double cVal_;
    
    // current s up and s down to hold temporary rsi calculations. gets replaced every time an update
    // value (candle) comes in recomputeIndicatorPoint;
    double cSUp_;
    double cSDown_;
    
    // keeps track of the num of inputs until
    // num inputs reaches period
    unsigned int size_;

};
