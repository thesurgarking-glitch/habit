#pragma once
#include "IPlug_include_in_plug_hdr.h"

enum EParams
{
  kDelayTime = 0,
  kFeedback,
  kScan,
  kModDepth,
  kModRate,
  kMix,
  kNumParams
};

class OnwardHabit : public iplug::Plugin
{
public:
  OnwardHabit(const iplug::InstanceInfo& info);
  void Reset() override;
  void OnParamChange(int paramIdx) override;
  void ProcessBlock(iplug::audio::AudioBuffer<float>& buffer) override;

private:
  double mSampleRate = 44100.0;
  std::vector<float> mBufferL, mBufferR;
  int mWritePos = 0;
  float mPhase = 0.f;
};
