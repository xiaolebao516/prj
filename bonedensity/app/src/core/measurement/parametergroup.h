#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QStringList>

// A parameter group is the set of measurement parameters that decide how SOS
// is produced: algorithm implementation, quality/pose/stability gates, probe
// distance D, SOS offset and channel choice. Gains, patient and session fields
// are recorded with each measurement but do not start a new group.
//
// Experiment logs measured with the same group share one folder
// <experiments>/<group id>/, which also holds parameters.json and a readable
// 参数说明.txt; every saved result carries its group id.
namespace ParameterGroup {

const QStringList& keys();

// The group-defining subset of a measurement/experiment config.
QJsonObject parameters(const QJsonObject& config);

// "<implementation>_<8 hex>", the hex part from the parameter values, so the
// same parameters always give the same id.
QString id(const QJsonObject& config);

// Creates or updates <experimentsRoot>/<id>/ with parameters.json and
// 参数说明.txt and returns the folder. newLog counts one more experiment log
// (one per round); measurements are counted once per measurement_session_id
// in the config (a log without one counts as its own measurement). Without
// newLog only the first/last use dates are kept current. Returns an empty
// string and sets *error on failure.
QString prepareFolder(const QString& experimentsRoot, const QJsonObject& config,
                      const QDateTime& when, bool newLog, QString* error = nullptr);

// Readable Chinese description of a parameters() object, one line per value.
QString describe(const QJsonObject& parameters);

} // namespace ParameterGroup
