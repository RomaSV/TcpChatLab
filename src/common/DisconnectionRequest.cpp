//
// Created by Roman Svechnikov on 12.09.2020.
//

#include <cstdlib>
#include "DisconnectionRequest.h"

std::pair<char *, size_t> DisconnectionRequest::serialize() const {
    size_t dataLength = sizeof(char);
    auto *data = static_cast<char *>(malloc(dataLength));
    data[0] = headers::DISCONNECTION_REQUEST;
    return std::pair<char *, size_t>(data, dataLength);
}
