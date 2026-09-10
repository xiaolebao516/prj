// Research only: compare production relock behavior at three warmup lengths.
#define MEASUREMENT_REPLAY_LIBRARY
#include "offline_replay.cpp"

static QVector<QJsonObject> validatedPrefix(const QString& path,
                                            const QString& expectedHash,
                                            bool& damaged)
{
    QFile file(path);
    require(file.open(QIODevice::ReadOnly), "Cannot read source");
    QCryptographicHash hash(QCryptographicHash::Sha256);
    require(hash.addData(&file), "Hash failed");
    require(QString::fromLatin1(hash.result().toHex()) == expectedHash,
            "Changed source");
    file.seek(0);
    QVector<QJsonObject> rows;
    damaged = false;
    while (!file.atEnd()) {
        const auto line = file.readLine(1024 * 1024);
        if (line.contains('\0') || !line.endsWith('\n')) {
            damaged = true;
            break;
        }
        QJsonParseError error;
        auto row = QJsonDocument::fromJson(line, &error).object();
        require(error.error == QJsonParseError::NoError && !row.isEmpty(),
                "Bad complete JSON line");
        for (const auto* key : {"raw_AD", "raw_AC", "raw_BD", "raw_BC"})
            row.remove(key);
        rows.append(row);
    }
    require(!rows.isEmpty() && rows.first()["event"] == "start", "Missing start");
    return rows;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString&) {});
    try {
        require(argc == 4,
                "Usage: warmup_policy_replay triage.json input-directory output.json");
        QFile manifest(QString::fromLocal8Bit(argv[1]));
        require(manifest.open(QIODevice::ReadOnly), "Cannot read manifest");
        const auto files = QJsonDocument::fromJson(manifest.readAll())
                               .object()["files"].toArray();
        require(files.size() == 73, "Expected complete triage manifest");

        const QVector<Case> variants{
            {"production-warmup14", .78, .78, -6, 6, 14, 10, true, true, true},
            {"production-warmup13", .78, .78, -6, 6, 13, 10, true, true, true},
            {"production-warmup12", .78, .78, -6, 6, 12, 10, true, true, true},
            {"production-warmup10", .78, .78, -6, 6, 10, 10, true, true, true}
        };
        QJsonArray scenarios;
        for (const auto& variant : variants) {
            QJsonArray attempts;
            int count = 0;
            for (const auto& value : files) {
                const auto entry = value.toObject();
                if (entry["file"].toString().mid(15, 2).toInt() < 8)
                    continue;
                bool damaged = false;
                const auto rows = validatedPrefix(
                    QDir(QString::fromLocal8Bit(argv[2]))
                        .filePath(entry["file"].toString()),
                    entry["sha256"].toString(), damaged);
                auto result = MainWindowSafetyTests::runFile(rows, variant, false, true);
                result["file"] = entry["file"];
                result["damaged_prefix_only"] = damaged;
                attempts.append(result);
                ++count;
            }
            require(count == 63, "Expected 63 afternoon logs");
            scenarios.append(QJsonObject{{"name", variant.name},
                                         {"warmup", variant.warmup},
                                         {"lock_need", 10},
                                         {"attempts", attempts}});
            std::fprintf(stderr, "%s complete\n", variant.name.toLocal8Bit().constData());
        }

        QFile output(QString::fromLocal8Bit(argv[3]));
        require(output.open(QIODevice::WriteOnly | QIODevice::NewOnly),
                "Output must be new");
        const QJsonObject root{
            {"scenarios", scenarios},
            {"scope", "Counterfactual replay on 63 hash-validated recorded trajectories. "
                      "Only warmup changes; lock need remains 10 and production relock, "
                      "A/D/G/SOS/round rules remain fixed. Not prospective hardware accuracy."}
        };
        const auto bytes = QJsonDocument(root).toJson();
        require(output.write(bytes) == bytes.size() && output.flush(), "Output failed");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Failed: %s\n", error.what());
        return 1;
    }
    return 0;
}
