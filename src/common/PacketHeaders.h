//
// Created by Roman Svechnikov on 12.09.2020.
//

#ifndef NETLABS_PACKETHEADERS_H
#define NETLABS_PACKETHEADERS_H

namespace headers {
    static constexpr char CONNECTION_REQUEST = 0x01;
    static constexpr char CONNECTION_RESPONSE = 0x02;
    static constexpr char DISCONNECTION_REQUEST = 0x03;
    static constexpr char CHAT_MESSAGE = 0x10;
}

#endif //NETLABS_PACKETHEADERS_H
