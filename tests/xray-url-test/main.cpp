#include <nekobox/sys/XrayURLTest.hpp>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <thread>
#include <chrono>
#include <iostream>
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    const auto args = app.arguments();
    QFile file(args[2]);
    if (!file.open(QIODevice::ReadOnly)) return 2;
    std::atomic<bool> cancel(false);
    if (args[5].toInt() == 0) cancel.store(true);
    std::thread canceller;
    if (args[5].toInt() > 0)
        canceller = std::thread([&cancel, delay = args[5].toInt()] {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            cancel.store(true);
        });
    const auto result = Configs::TestXrayURL(args[1], QJsonDocument::fromJson(file.readAll()).object(),
                                            QUrl(args[3]), args[4].toInt(), cancel,
                                            qEnvironmentVariable("NEKOBOX_TEST_CA"));
    if (canceller.joinable()) canceller.join();
    std::cout << QJsonDocument(QJsonObject{{"latency", result.latencyMs}, {"error", result.error},
                                         {"cancelled", result.cancelled}}).toJson().toStdString();
}
