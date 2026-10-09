#pragma once
#include "measurement/measurementtypes.h"

#include <QJsonObject>
#include <QString>

class SignalProcessor;

// Algorithm switches that tell the formal build from the isolated research
// trial builds (CONFIG+=<trial>, see src/core/core.pri). This is the only
// place that reads the BONE_*_EXPERIMENT defines.
struct MeasurementProfile {
    QString implementation;   // recorded with every result and experiment log
    QString title;            // main window title
    bool enforceBOnsetConsistency = true;
    // A quality is the lower of its front and middle correlation windows at
    // the selected A lag.
    bool useDualWindowAQuality = true;
    bool completeTruncatedBPeak = false;
    // After a partial round loses lock, keep its values until the next lock
    // and only if that lock is the same cluster.
    bool deferPartialDiscardUntilRelock = true;
    // Frames failing G may still update the lag-stability observation.
    bool observeStabilityBeforeG = true;

    int clippedBPeakExtension() const { return completeTruncatedBPeak ? 15 : 0; }

    static MeasurementProfile current();
};

// The parameters that decide how SOS is produced (see ParameterGroup).
QJsonObject measurementParameters(const MeasureConfig& config, const MeasurementProfile& profile,
                                  const SignalProcessor& processor);
