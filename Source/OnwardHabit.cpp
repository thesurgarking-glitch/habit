#include "OnwardHabit.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

using namespace iplug;
using namespace igraphics;

OnwardHabit::OnwardHabit(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, 1))
{
  GetParam(kDelayTime)->InitDouble("Delay Time", 0.5, 0.01, 2.0, 0.01, "s");
  GetParam(kFeedback)->InitDouble("Feedback", 0.4, 0.0, 0.95, 0.01);
  GetParam(kScan)->InitDouble("Scan", 0.5, 0.0, 1.0, 0.01);
  GetParam(kModDepth)->InitDouble("Mod Depth", 0.0, 0.0, 0.05, 0.001);
  GetParam(kModRate)->InitDouble("Mod Rate", 0.25, 0.01, 5.0, 0.01, "Hz");
  GetParam(kMix)->InitDouble("Mix", 0.5, 0.0, 1.0, 0.01);

  const IRECT b = GetBounds();
  AttachGraphics(MakeGraphics(this, b));
  GetUI()->AttachPanelBackground(COLOR_DARK_SLATE_GRAY);
  AttachControl(new IKnobControl(b.GetCentredInside(60).GetVShifted(-120), kDelayTime));
  AttachControl(new IKnobControl(b.GetCentredInside(60).GetVShifted(-40), kFeedback));
  AttachControl(new IKnobControl(b.GetCentredInside(60).GetVShifted(40), kScan));
  AttachControl(new IKnobControl(b.GetCentredInside(60).GetVShifted(120), kMix));
}

void OnwardHabit::Reset()
{
  mSampleRate = GetSampleRate();
  int maxDelaySamples = (int)(mSampleRate * 180.f); // 3 min buffer
  mBufferL.assign(maxDelaySamples, 0.f);
  mBufferR.assign(maxDelaySamples, 0.f);
  mWritePos = 0;
  mPhase = 0.f;
}

void OnwardHabit::OnParamChange(int) {}

void OnwardHabit::ProcessBlock(audio::AudioBuffer<float>& buffer)
{
  int nFrames = buffer.GetNFrames();
  int nChans = buffer.GetNChannels();
  int maxDelaySamples = (int)mBufferL.size();

  float delayTimeSec = (float)GetParam(kDelayTime)->Value();
  int delaySamples = (int)(delayTimeSec * mSampleRate);

  float feedback = (float)GetParam(kFeedback)->Value();
  float scan = (float)GetParam(kScan)->Value();
  float modDepth = (float)GetParam(kModDepth)->Value();
  float modRate = (float)GetParam(kModRate)->Value();
  float mix = (float)GetParam(kMix)->Value();

  for (int s = 0; s < nFrames; s++)
  {
    float l = buffer.GetChannelData(0)[s];
    float r = nChans > 1 ? buffer.GetChannelData(1)[s] : l;

    // LFO modulation
    mPhase += 2.0 * M_PI * modRate / mSampleRate;
    if (mPhase > 2.0 * M_PI) mPhase -= 2.0 * M_PI;
    float mod = sin(mPhase) * modDepth * mSampleRate;

    int readPos = (mWritePos - delaySamples - (int)(scan * 1000) + (int)mod + maxDelaySamples) % maxDelaySamples;

    float dl = mBufferL[readPos];
    float dr = mBufferR[readPos];

    mBufferL[mWritePos] = l + dl * feedback;
    mBufferR[mWritePos] = r + dr * feedback;

    float wetL = dl;
    float wetR = dr;

    buffer.GetChannelData(0)[s] = l * (1.f - mix) + wetL * mix;
    if (nChans > 1)
      buffer.GetChannelData(1)[s] = r * (1.f - mix) + wetR * mix;

    mWritePos++;
    if (mWritePos >= maxDelaySamples) mWritePos = 0;
  }
}
