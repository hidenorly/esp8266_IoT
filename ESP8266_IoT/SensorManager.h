#ifndef __SENSORMANAGER_H__
#define __SENSORMANAGER_H__

#include "base.h"
#include "SensorBase.h"
#include "TemplateArray.h"
#include "LooperThreadTicker.h"

class SensorData
{
  public:
    SensorData(float rateHz=1.0f, int windowSize=100);
    virtual ~SensorData();

    float getSamplingRate(void){ return mRateHz; }
    float getWindowSize(void){ return mWindowSize; }

  protected:
    float mRateHz;
    int mWindowSize;
    int mCurrentPos;
    time_t mCurrentTime;

    int getNextDataPos(void);

    class Poller:public TimerContextTicker
    {
      public:
        Poller(int dutyMSec=1000);
      protected:
        virtual void doCallback(void);
    };
    Poller* mpPoller;
};

template <typename T>
class TSensorData:public SensorData
{
  public:
    TSensorData(T rateHz=1.0f, int windowSize=100);
    ~TSensorData();

    virtual void addData(T data);
    T operator[](int nPos);

  protected:
    T* mpData;
};

extern template class TSensorData<int>;
extern template class TSensorData<float>;
typedef TSensorData<int> SensorDataInt;
typedef TSensorData<float> SensorDataFloat;

class SensorManager
{
  public:
    static SensorManager* getInstance(void);
    static void releaseInstance(void);
    void addSensor(ISensor* pSensor, SensorData* pData);
    SensorData* getSensorDataReference(ISensor* pSensor);

  protected:
    SensorManager();
    ~SensorManager();
    static void cleanUp(void);

    static TemplateArray<ISensor> mpSensors;

    static SensorManager* mpThis;
    static int mRefCount;
};

#endif // __SENSORMANAGER_H__
