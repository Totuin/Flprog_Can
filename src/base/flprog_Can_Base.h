
#include <Arduino.h>
#include "flprogUtilites.h"

#define FLPROG_CAN_BUS_NORMAL_MODE 0
#define FLPROG_CAN_BUS_LOOPBACK_MODE 1
#define FLPROG_CAN_BUS_SILENT_MODE 2
#define FLPROG_CAN_BUS_SILENT_LOOPBACK_MODE 3

#define FLPROG_CAN_BUS_MESSAGE_CHANGE_MODE 0
#define FLPROG_CAN_BUS_MESSAGE_PERIODICALLY_MODE 1
#define FLPROG_CAN_BUS_MESSAGE_CHANGE_PERIODICALLY_MODE 2
#define FLPROG_CAN_BUS_MESSAGE_READ_MODE 3
#define FLPROG_CAN_BUS_MESSAGE_SPY_MODE 4

typedef struct CAN_message_t
{
    uint32_t id = 0;        // can identifier
    uint16_t timestamp = 0; // time when message arrived
    uint8_t idhit = 0;      // filter that id came from
    struct
    {
        bool extended = 0; // identifier is extended (29-bit)
        bool remote = 0;   // remote transmission request packet type
        bool overrun = 0;  // message overrun
        bool reserved = 0;
    } flags;
    uint8_t len = 8;      // length of data
    uint8_t buf[8] = {0}; // data
    int8_t mb = 0;        // used to identify mailbox reception
    uint8_t bus = 1;      // used to identify where the message came (CAN1, CAN2 or CAN3)
    bool seq = 0;         // sequential frames
} CAN_message_t;

class FLProgAbstractCanBus
{
public:
    virtual bool hasNewReadMessage() { return false; };
    virtual CAN_message_t *getReadMessage() { return 0; };
    virtual bool write(CAN_message_t &CAN_tx_msg, bool sendMB = false) { return false; };

protected:
};

class FLProgCanBusMessage
{
public:
    FLProgCanBusMessage(FLProgAbstractCanBus *cunBus, uint32_t id, bool extended = false, uint8_t len = 8);
    void setMode(uint8_t mode);
    void setSendPeriod(uint32_t sendPeriod) { _sendPeriod = sendPeriod; };
    void send(bool value);
    void setData(uint8_t index, uint8_t value);
    void id(uint32_t value) { _message.id = value; };
    void extended(bool value) { _message.flags.extended = value; };
    void len(uint8_t value);

    uint8_t getMode() { return _mode; };
    uint32_t getSendPeriod() { return _sendPeriod; };
    uint8_t getData(uint8_t index);
    bool hasNewData() { return _hasNewData; };
    CAN_message_t *message() { return &_message; };
    uint32_t id() { return _message.id; };
    bool extended() { return _message.flags.extended; };
    uint8_t len() { return _message.len; };

    void pool();

private:
    void readPool();
    void sendPool();
    void checkNeedSend();
    CAN_message_t _message;
    FLProgAbstractCanBus *_cunBus = 0;
    bool _isNeedSend = true;
    bool _hasNewData = false;
    bool _sendOldValue = false;
    uint8_t _mode = FLPROG_CAN_BUS_MESSAGE_READ_MODE;
    uint32_t _sendPeriod = 50;
    uint32_t _sendTime = flprog::timeBack(_sendPeriod);
};