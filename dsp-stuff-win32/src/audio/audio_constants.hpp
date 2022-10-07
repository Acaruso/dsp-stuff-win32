#pragma once

// TODO: set these dynamically at app startup time
const unsigned samplesPerSecond = 48000;
const unsigned samplesPerMs = 48;
const float secondsPerSample = 1.0f / samplesPerSecond;
const int bufferSize = samplesPerSecond / 100;
