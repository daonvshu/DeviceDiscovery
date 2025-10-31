#pragma once

#include <qcryptographichash.h>
#include <qrandom.h>

#include "global.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

static QByteArray hmac(const QByteArray &key, const QByteArray &message, QCryptographicHash::Algorithm algorithm) {
    // get block size
    int blockSize;
    switch (algorithm) {
        case QCryptographicHash::Sha1:
        case QCryptographicHash::Md5:
        case QCryptographicHash::Sha256:
            blockSize = 64; // 512 bit
            break;
        case QCryptographicHash::Sha512:
            blockSize = 128; // 1024 bit
            break;
        default:
            blockSize = 64; // default 64
            break;
    }

    QByteArray k = key;
    if (k.size() > blockSize) {
        k = QCryptographicHash::hash(k, algorithm);
    }
    if (k.size() < blockSize) {
        k.append(QByteArray(blockSize - k.size(), char(0x00)));
    }

    QByteArray o_key_pad(blockSize, char(0x5c));
    QByteArray i_key_pad(blockSize, char(0x36));

    for (int i = 0; i < blockSize; ++i) {
        o_key_pad[i] = o_key_pad[i] ^ k[i];
        i_key_pad[i] = i_key_pad[i] ^ k[i];
    }

    QByteArray innerHash = QCryptographicHash::hash(i_key_pad + message, algorithm);
    QByteArray hmacResult = QCryptographicHash::hash(o_key_pad + innerHash, algorithm);

    return hmacResult;
}

static QByteArray generateNonce(int length = 16) {
    QByteArray nonce;
    nonce.resize(length);
    for (int i = 0; i < length; ++i) {
        nonce[i] = char(QRandomGenerator::global()->bounded(0, 256));
    }
    return nonce;
}

DEVICE_DISCOVERY_END_NAMESPACE