#pragma once
#include <QHostAddress>
#include <QUrl>
#include <QRegularExpression>

namespace Configs {
inline QString NormalizeTunRoutePrefix(const QString &input) {
    const auto value = input.trimmed();
    const auto slash = value.indexOf('/');
    const auto host = slash < 0 ? value : value.left(slash);
    QHostAddress address;
    if (!address.setAddress(host) || !address.scopeId().isEmpty()) return {};

    const int maxPrefix = address.protocol() == QAbstractSocket::IPv4Protocol ? 32 : 128;
    int prefix = maxPrefix;
    if (slash >= 0) {
        const auto length = value.mid(slash + 1);
        static const QRegularExpression digits(QStringLiteral("^[0-9]{1,3}$"));
        if (!digits.match(length).hasMatch()) return {};
        prefix = length.toInt();
        if (prefix > maxPrefix) return {};
    }
    return address.toString() + '/' + QString::number(prefix);
}

inline QString NormalizeExclusionAddress(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) return {};
    QHostAddress address;
    if (address.setAddress(value)) return address.toString();
    if (value.contains('/') && !value.contains("://")) {
        const auto subnet = QHostAddress::parseSubnet(value);
        if (!subnet.first.isNull() && subnet.second >= 0)
            return subnet.first.toString() + "/" + QString::number(subnet.second);
        return {};
    }
    if (value.contains("://")) {
        const QUrl url(value, QUrl::StrictMode);
        if (!url.isValid() || url.host().isEmpty()) return {};
        value = url.host();
        if (address.setAddress(value)) return address.toString();
    }
    if (value.startsWith("*.")) value.remove(0, 2);
    if (value.endsWith('.')) value.chop(1);
    value = QString::fromLatin1(QUrl::toAce(value)).toLower();
    static const QRegularExpression domain(
        "^(?=.{1,253}$)[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?(?:\\.[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?)*$");
    if (!domain.match(value).hasMatch()) return {};
    static const QRegularExpression numeric("^[0-9.]+$");
    return numeric.match(value).hasMatch() ? QString() : value;
}
}
